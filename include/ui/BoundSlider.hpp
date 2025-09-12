// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace modal::ui {
    /** @brief Slider with binding to ValueTreeState parameter
     *
     * Has slider and label with name of parameter
     */
    class BoundSlider : public juce::Component {
     public:
        /// @private
        juce::Slider slider;
        /// @private
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
        /// @private
        juce::Font label_font{juce::FontOptions{14}};
        /// @private
        juce::Label label;
        /// @private
        juce::Slider::SliderStyle style;

        /** @brief Constructor
         * @param s Style of slider (vertical or horizontal)
         */
        explicit BoundSlider(const juce::Slider::SliderStyle s) : style(s) {
            label.setFont(label_font);
        }

        /** @brief Setup binding to parameter
         * @param plug_params Reference to ValueTree of plugin processor
         * @param param_id Name of parameter to bind to
         * @param skew Enable skew factor
         * @param skew_midpoint Skew midpoint (see juce docs for `setSkewFactorFromMidPoint()`)
         */
        void setup(juce::AudioProcessorValueTreeState& plug_params, const juce::String& param_id, bool skew = false,
                   double skew_midpoint = -1.0);

        /** @brief Override of function on base class.
         */
        void paint(juce::Graphics& g) override;

        /** @brief Override of function on base class.
         */
        void resized() override;
    };
}
