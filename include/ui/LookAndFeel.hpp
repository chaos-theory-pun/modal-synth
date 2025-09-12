// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace modal::ui {
    /** @brief Custom look and feel
     *
     * Based on `LookAndFeel_V4`, in order to have more proportional sizing of widgets
     * when the window size changes
     */
    class ModalLookAndFeel : public juce::LookAndFeel_V4 {
        juce::Font font = juce::FontOptions{};
     public:
        /** @brief Call when window width changes
         *
         * Resizes font to match new size
         * @param window_width New window width
         */
        void setWindowWidth (int window_width) {
            float pts = (11.f / 600.f) * static_cast<float>(window_width);
            font = juce::FontOptions{}.withPointHeight(pts);
        }

        /** @brief Override of function on base class.
         */
        juce::Font getLabelFont(juce::Label&) override {
            return font;
        }

        /** @brief Override of function on base class.
         */
        juce::Font getComboBoxFont(juce::ComboBox&) override {
            return font;
        }

    };
}