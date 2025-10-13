// SPDX-License-Identifier: GPL-3.0-or-later

#include <ui/MacroController.hpp>
#include <print>

namespace modal::ui {
    MacroController::MacroController(juce::AudioProcessor& processor, juce::AudioProcessorValueTreeState& tree) {
        for (const auto& p : processor.getParameters()) {
            auto p2 = dynamic_cast<juce::RangedAudioParameter*>(p);
            if (!p2->isMetaParameter() && typeid(*p2) == typeid(juce::AudioParameterFloat)) {
                const auto p3 = dynamic_cast<juce::AudioParameterFloat*>(p);
                const auto id = p2->getParameterID();
                const auto name = p2->getName(32);
                // std::println("param:\t{}\t(non-meta float)", id.toStdString());
                params.emplace_back(id, name, *p3, tree.getParameterAsValue(id));
            }
        }
        // std::println("items {}", params.size());
    }

    void MacroController::set_values(modal::dsp::num) {
    }

    MacroController::UI& MacroController::get_ui() {
        // UI ui {*this};
        return ui;
    }

    void MacroController::UI::setup() {
        for (auto& setting : settingses) {
            setting.setup();
            addAndMakeVisible(setting);
        }
    }

    void MacroController::UI::resized() {
        juce::FlexBox fb;
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        fb.justifyContent = juce::FlexBox::JustifyContent::center;
        fb.alignContent = juce::FlexBox::AlignContent::stretch;
        fb.flexDirection = juce::FlexBox::Direction::column;

        juce::Array<juce::FlexItem> items {};

        for (auto& setting : settingses) {
            items.add(juce::FlexItem{setting}.withFlex(1));
        }

        fb.items = items;

        fb.performLayout(getLocalBounds());
    }


    void MacroController::MacroSettings::setup() {
        for (size_t i = 0; i < parent.params.size(); i++) {
            const auto& pi = parent.params[i];
            options.addItem(pi.name, static_cast<int>(i) + 1);
        }

        options.addListener(&parent);
        addAndMakeVisible(options);

        lo.setComponentID("mod_low");
        lo.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        lo.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 140, 20);
        addAndMakeVisible(lo);

        hi.setComponentID("mod_high");
        hi.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        hi.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 140, 20);
        addAndMakeVisible(hi);
    }

    void MacroController::MacroSettings::resized() {
        juce::FlexBox fb;
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        fb.justifyContent = juce::FlexBox::JustifyContent::center;
        fb.alignContent = juce::FlexBox::AlignContent::stretch;
        fb.flexDirection = juce::FlexBox::Direction::row;

        fb.items = {
            juce::FlexItem{options}.withFlex(1).withHeight(40).withAlignSelf(juce::FlexItem::AlignSelf::center),
            juce::FlexItem{lo}.withFlex(1),
            juce::FlexItem{hi}.withFlex(1)
        };

        fb.performLayout(getLocalBounds());
    }

    // does not bring across the convert lambda functions
    template<typename I, typename O>
    juce::NormalisableRange<O> convert_nr(const juce::NormalisableRange<I>& in) {
        juce::NormalisableRange<O> out;
        out.start = static_cast<O>(in.start);
        out.end = static_cast<O>(in.end);
        out.interval = static_cast<O>(in.interval);
        out.skew = static_cast<O>(in.skew);
        out.symmetricSkew = in.symmetricSkew;
        return out;
    }

    void MacroController::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
        const auto parent = comboBoxThatHasChanged->getParentComponent();
        const auto i = static_cast<size_t>(comboBoxThatHasChanged->getSelectedId() - 1);
        const auto param = params[i];

        const auto range = convert_nr<float, double>(param.pm.getNormalisableRange());
        const auto lo_slider = dynamic_cast<juce::Slider*>(parent->findChildWithID("mod_low"));
        const auto hi_slider = dynamic_cast<juce::Slider*>(parent->findChildWithID("mod_high"));
        lo_slider->setNormalisableRange(range);
        hi_slider->setNormalisableRange(range);
    }
}
