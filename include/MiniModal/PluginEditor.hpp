// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include <MiniModal/PluginProcessor.hpp>
#include <ui/BoundCombobox.hpp>
#include <ui/BoundSlider.hpp>
#include <ui/LookAndFeel.hpp>
#include <ui/MacroController.hpp>

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
        ui::ModalLookAndFeel laf;
        juce::AudioProcessorValueTreeState& params;
        juce::ComponentBoundsConstrainer constraints;

#ifdef MODAL_DEBUG_UI
        melatonin::Inspector inspector{*this, false};
#endif

        struct Controls final : juce::Component {
            ui::BoundCombobox foldback_mode;
            ui::BoundSlider foldback_point{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider num_modes{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider decay{Slider::RotaryHorizontalVerticalDrag};

            void setup(AudioProcessorValueTreeState& plug_params);

            void paint(Graphics& g) override;

            void resized() override;

        };

        Controls controls;

        struct SpectrumControls final : juce::Component {
            ui::BoundSlider detune{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider exponent{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider falloff{Slider::RotaryHorizontalVerticalDrag};
            ui::BoundSlider even_gain{Slider::RotaryHorizontalVerticalDrag};

            Label l{"sliders_label", "Spectrum Controls"};

            void setup(AudioProcessorValueTreeState& plug_params);

            void paint(Graphics& g) override;

            void resized() override;
        };

        SpectrumControls spectrum_controls;

        struct ExciterControls final : juce::Component {
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

        struct MacroControls final : public juce::Component {
            ui::BoundSlider macro_dial {Slider::RotaryHorizontalVerticalDrag};
            ui::MacroController::UI& macro;
            explicit MacroControls(ui::MacroController::UI& m) : macro{m} {}

            void setup(AudioProcessorValueTreeState& plug_params);
            void resized() override;
        };

        MacroControls macro_controls {processorRef.mediator.get_ui()};

        MidiKeyboardComponent keyboard {processorRef.keyboard_state, juce::KeyboardComponentBase::horizontalKeyboard};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiniEditor)
    };
}