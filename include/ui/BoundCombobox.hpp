// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace modal::ui {
    class BoundCombobox : public juce::ComboBox {
        public:
            std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;

            explicit BoundCombobox() : ComboBox({}) {}

            void setup(juce::AudioProcessorValueTreeState& plug_params, const juce::String& param_id);

    };
}