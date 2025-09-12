// SPDX-License-Identifier: GPL-3.0-or-later

#include <ui/BoundCombobox.hpp>

void modal::ui::BoundCombobox::setup(juce::AudioProcessorValueTreeState& plug_params, const juce::String& param_id) {
    const auto combos = dynamic_cast<juce::AudioParameterChoice*>(plug_params.getParameter(param_id));

    addItemList(combos->choices, 1);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(plug_params, param_id, *this);
}
