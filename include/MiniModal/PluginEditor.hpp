// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include <MiniModal/PluginProcessor.hpp>
#include <ui/BoundCombobox.hpp>
#include <ui/BoundSlider.hpp>
#include <ui/LookAndFeel.hpp>

#ifdef MODAL_DEBUG_UI
#include "melatonin_inspector/melatonin_inspector.h"
#endif

namespace modal::plugin {
    using namespace juce; //todo: fix namespaces
//==============================================================================
    class MiniEditor final : public juce::AudioProcessorEditor {
     public:
        explicit MiniEditor(MiniProcessor&, juce::AudioProcessorValueTreeState& p);

        ~MiniEditor() override;

        //==============================================================================
        void paint(juce::Graphics&) override;

        void resized() override;

     private:
        // This reference is provided as a quick way for your editor to
        // access the processor object that created it.
        MiniProcessor& processorRef;
        juce::AudioProcessorValueTreeState& params;
        juce::ComponentBoundsConstrainer constraints;
        ui::ModalLookAndFeel laf;

#ifdef MODAL_DEBUG_UI
        melatonin::Inspector inspector{*this, false};
#endif

        struct Controls : juce::Component {
            ui::BoundCombobox foldback_mode;
            ui::BoundSlider foldback_point{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider num_modes{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider detune{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider exponent{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider falloff{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider decay{Slider::RotaryHorizontalDrag};

            void setup(AudioProcessorValueTreeState& plug_params);

            void paint(Graphics& g) override;

            void resized() override;

        };

        Controls controls;

        struct Sliders : juce::Component {
            ui::BoundSlider slider1{Slider::LinearVertical};
            ui::BoundSlider slider2{Slider::LinearVertical};

            ui::BoundSlider dial1{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider dial2{Slider::RotaryHorizontalVerticalDrag};

            Label l{"sliders_label", "Mode Offsets"};


            void setup(AudioProcessorValueTreeState& plug_params);

            void paint(Graphics& g) override;

            void resized() override;
        };

        Sliders sliders;

        struct ExciterControls : juce::Component {
            ui::BoundCombobox exciter_mode;
            ui::BoundSlider exciter_rate{Slider::RotaryHorizontalVerticalDrag};

            ui::BoundSlider attack{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider release{Slider::RotaryHorizontalVerticalDrag};

            Label l{"exciter_label", "Exciter"};

            void setup(AudioProcessorValueTreeState& plug_params);

            void paint(Graphics& g) override;

            void resized() override;
        };

        ExciterControls exciter_controls;

        MidiKeyboardComponent keyboard {processorRef.keyboard_state, juce::KeyboardComponentBase::horizontalKeyboard};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiniEditor)
    };
}