// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <juce_gui_basics/juce_gui_basics.h>

#include "dsp/dsp.hpp"

namespace modal::ui {
    class MacroController final : public juce::ComboBox::Listener {
        struct ParamInfo {
            juce::String id;
            juce::String name;
            juce::AudioParameterFloat& pm;
        };

        std::vector<ParamInfo> params;

        public:
            explicit MacroController(const juce::AudioProcessor& processor);
            void set_values(modal::dsp::num n) const;

            class MacroSettings final : public juce::Component {
                friend class MacroController;
                juce::ComboBox options;
                juce::Slider lo {"mod_low"}, hi {"mod_high"};
                MacroController& parent;
                explicit MacroSettings(MacroController& p) : parent{p} {}
                void add_params();
             public:
                MacroSettings(const MacroSettings& old) : parent {old.parent} {}
                void setup();
                void resized() override;
            };

            class UI final : public juce::Component {
                friend class MacroController;
                std::array<MacroSettings, 3> settingses;
                MacroController& parent;
                explicit UI(MacroController& p) : settingses {MacroSettings{p}, MacroSettings{p}, MacroSettings{p}},
                                                  parent {p} {}
                public:
                    UI(const UI& old) : settingses{old.settingses}, parent{old.parent} {}
                    void setup();
                    void resized() override;

            };

            UI ui {*this};

            UI& get_ui();

            juce::ValueTree dump_state() const;
            void load_state(const juce::ValueTree& state);

            void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    };
}
