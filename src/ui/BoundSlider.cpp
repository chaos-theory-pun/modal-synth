// SPDX-License-Identifier: GPL-3.0-or-later

#include <ui/BoundSlider.hpp>

namespace modal::ui {
    void BoundSlider::setup(juce::AudioProcessorValueTreeState& plug_params, const juce::String& param_id, bool skew,
                            double skew_midpoint) {
        auto param = plug_params.getParameter(param_id);

        slider.setMouseClickGrabsKeyboardFocus(false);
        slider.setSliderStyle(style);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 140, 20);
        if (skew) {
            slider.setSkewFactorFromMidPoint(skew_midpoint);
        }
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(plug_params, param_id,
                                                                                            slider);
        label.setText(param->getName(30), juce::dontSendNotification);
        label.setMouseClickGrabsKeyboardFocus(false);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
        addAndMakeVisible(slider);
    }

    void BoundSlider::paint(juce::Graphics& g) {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void BoundSlider::resized() {
        juce::FlexBox fb;
        fb.flexDirection = juce::FlexBox::Direction::column;
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        fb.justifyContent = juce::FlexBox::JustifyContent::center;
        fb.alignContent = juce::FlexBox::AlignContent::center;

        auto width = (1) * static_cast<float>(getWidth());
        auto text_height = (1.f / 4.f) * static_cast<float>(getHeight());
        // auto font_height = (14.f / 700.f) * static_cast<float>(getTopLevelComponent()->getWidth());

        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false,
                               static_cast<int>(width / 3), static_cast<int>(text_height));
        // resize font based on size in document, assuming font creation is expensive and font setting is cheap
        // label_font.setHeight(font_height);
        // label.setFont(label_font);

        fb.items.add(juce::FlexItem(label).withWidth(width).withHeight(text_height));
        fb.items.add(juce::FlexItem(slider).withFlex(1).withMaxWidth(width).withMaxHeight(getHeight() - text_height));

        fb.performLayout(getLocalBounds());
    }
}
