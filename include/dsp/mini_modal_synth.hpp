// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <array>

#include <dsp/dsp.hpp>
#include "resonator.hpp"
#include <dsp/mod.hpp>
#include <randutils.hpp>
#include <dsp/osc.hpp>

#include <dsp/formant.hpp>

namespace modal::dsp::synth {
    /// @brief Kinds of exciter for the modal synth
    enum class MiniModalExiterKind {
        /// Impulse (tone will decay).
        Impulse = 0,
        /// White noise
        Noise = 1,
        /// Pitched impulse train
        Impulses = 2
    };

    /// @brief Spectrum foldback modes for the modal synth
    enum class MiniModalFoldbackKind {
        /// Modes will not sound at or above the Nyquist frequency
        NyquistStop = 0,
        /// Modes will sound as undertones (successive modes lower than the root).
        Undertones = 1,
        /// Modes will be reflected/aliased around a set frequency
        Foldback = 2
    };

    /// @private
    struct FoldbackSettings {
        MiniModalFoldbackKind mode = MiniModalFoldbackKind::NyquistStop;
        modal::dsp::num foldback_point = 1600;
    };

    /**
     * @brief Modal synthesiser
     *
     * This is the implementation of the main synthesiser in the plugin.
     * Has a set of `physical::filters::PhasorResonator` modes, `osc::Phasor` exciters,
     * an `mod::AHREnv` envelope, and a `physical::FormantFilter` filter.
     *
     * Some member functions require the mode coefficients to be updated after they are called.
     * This is an expensive operation, so these functions do not update the coefficients themselves,
     * and require the caller to update the coefficients using `update_mode_coefficients()` after.
     * This is noted in the documentation of those functions.
     *
     * Is an [instrument class](docs/DSP Coding Standards.md).
     * @tparam maxModes Maximum number of modes to synthesise
     */
    template<size_t maxModes>
    class MiniModalSynth {
        std::array<physical::filters::PhasorResonator, maxModes> modes;
        size_t currentModes = maxModes;
        modal::dsp::num inharmonicity = 0;
        modal::dsp::num exponent = 0;
        modal::dsp::num freq = 0;
        modal::dsp::num velocity = 1;
        modal::dsp::num decay = 1;
        modal::dsp::num falloff = 1;
        modal::dsp::num even_gain = 1;

        MiniModalExiterKind exciter = MiniModalExiterKind::Noise;
        modal::dsp::num exciter_rate = 20;
        modal::dsp::osc::Phasor osc_exciter {48000};
        FoldbackSettings foldback;

        modal::dsp::mod::AHREnv env;
        randutils::default_rng noise;

     public:
        /** @brief Note on.
         *
         * Behaves as described in [the DSP coding standards](docs/DSP Coding Standards.md).
         *
         * Updates mode coefficients.
         *
         * \param key_freq Note to play, as Hz
         * \param vel Velocity of note, in range 0-1
         */
        void on(modal::dsp::num key_freq, modal::dsp::num vel) {
            freq = key_freq;
            velocity = vel;
            update_mode_coefficients();
            switch (exciter) {
                case MiniModalExiterKind::Impulse:
                    ping();
                    break;
                case MiniModalExiterKind::Noise:
                case MiniModalExiterKind::Impulses:
                    env.on();
                    break;
            }
        }

        /** @brief Note off.
         *
         * Behaves as described in [the DSP coding standards](docs/DSP Coding Standards.md)
         */
        void off() {
            switch (exciter) {
                case MiniModalExiterKind::Impulse:
                    break;
                case MiniModalExiterKind::Noise:
                case MiniModalExiterKind::Impulses:
                    env.off();
                    break;
            }
        }

        /** @brief Synthesises a single audio sample.
         *
         * Behaves as described in [the DSP coding standards](docs/DSP Coding Standards.md)
         */
        modal::dsp::num tick() {
            // modal::dsp::num to_mode =  * env.tick();
            modal::dsp::num to_mode = 0;

            osc_exciter.tick();

            switch (exciter) {
                case MiniModalExiterKind::Noise:
                    to_mode = noise.uniform(-0.05_nm, 0.05_nm);
                    break;
                case MiniModalExiterKind::Impulses:
                    to_mode = osc::impulse_train(osc_exciter) * 0.6_nm;
                    break;
                case MiniModalExiterKind::Impulse:
                    break;
            }

            to_mode *= env.tick();

            modal::dsp::num modes_out = 0;
            for (size_t i = 0; i < currentModes; i++) {
                modes_out += modes[i].tick(to_mode);
            }

            modal::dsp::num out = modes_out;

            out *= (velocity * velocity);

            return out;
        }

        /** @brief Sets the exciter.
         *
         * @param new_exciter New exciter type
         */
        void set_exciter(const MiniModalExiterKind new_exciter) {
            if (exciter == MiniModalExiterKind::Noise && new_exciter != MiniModalExiterKind::Noise) {
                env.reset();
            }
            exciter = new_exciter;
        }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
        /** @brief Sets coefficients related to the spectrum of modes.
         *
         * Requires updating coefficients.
         * @param num_modes Number of modes to synthesise (must be equal or less than `NumModes`)
         * @param inharm Linear inharmonicity factor, usually between -0.06 and 2
         * @param expo Exponential inharmonicity factor, usually between 0.1 and 10
         * @param e_rate Rate or pitch of exciter, in Hz
         * @param dcy Decay time, in seconds
         * @param flof Exponential falloff of increasing modes, usually between 0 and 3
         * @return If coefficients need to be updated
         */
        bool set_params(size_t num_modes, modal::dsp::num inharm, modal::dsp::num expo, modal::dsp::num e_rate, modal::dsp::num dcy, modal::dsp::num flof, modal::dsp::num eg) {
            bool changed = false;
            if (num_modes != currentModes || inharm != inharmonicity
                || expo != exponent || e_rate != exciter_rate
                || dcy != decay || flof != falloff || eg != even_gain) {
                changed = true;
            }
            currentModes = num_modes;
            inharmonicity = inharm;
            exponent = expo;
            exciter_rate = e_rate;
            decay = dcy;
            falloff = flof;
            even_gain = eg;
            return changed;
        }
#pragma clang diagnostic pop

        /** @brief Sets the timings for the envelope of the exciter.
         * @param attack Attack time, in seconds
         * @param release Release time, in seconds
         */
        void set_env_params(const modal::dsp::num attack, const modal::dsp::num release) {
            env.set_params(attack, release);
        }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
        /** @brief Set foldback mode.
         *
         * Requires updating coefficients.
         * @param mode `ModalFoldbackKind`, foldback mode for the spectrum
         * @param foldback_point Point to mirror the spectrum when set to `Foldback`, in Hz
         * @return If coefficients need to be updated
         */
        bool set_foldback_settings(const MiniModalFoldbackKind mode, modal::dsp::num foldback_point) {
            const bool changed = foldback.mode != mode || foldback.foldback_point != foldback_point;
            foldback.mode = mode;
            foldback.foldback_point = foldback_point;
            return changed;
        }
#pragma clang diagnostic pop

        /** @brief Sets the internal sample rate of the synthesiser.
         *
         * Behaves as described in [the DSP coding standards](docs/DSP Coding Standards.md)
         */
        void set_sample_rate(modal::dsp::num sr) {
            for (auto& m : modes) {
                m.set_sample_rate(sr);
            }
            env.set_sample_rate(sr);
            osc_exciter.set_sample_rate(sr);
        }

        /** @brief Update the internal coefficients of the modal filters
         * to use the updated parameters.
         *
         * Can be expensive, so don't call unnecessarily.
         */
        void update_mode_coefficients() {
            switch (foldback.mode) {
                case MiniModalFoldbackKind::NyquistStop: {
                    for (size_t i = 0; i < currentModes; i++) {
                        num mode_idx = static_cast<num>(i); // i
                        num mode_idx_p1 = mode_idx + 1; // k
                        num mode_gain = i % 2 == 1 ? even_gain : 1_nm;
                        modal::dsp::num overtone = mode_idx_p1 * (1 + mode_idx * (inharmonicity));
                        modal::dsp::num mode_freq = freq * std::pow(overtone, exponent);
                        modal::dsp::num distance = (2.0_nm / std::pow((mode_idx + 1.0_nm), falloff)) * mode_gain;
                        modes[i].set_params(mode_freq, distance, distance * decay);
                    }
                    break;
                }
                case MiniModalFoldbackKind::Undertones: {
                    for (size_t i = 0; i < currentModes; i++) {
                        num mode_idx = static_cast<num>(i); // i
                        num mode_idx_p1 = mode_idx + 1; // k
                        num mode_gain = i % 2 == 1 ? even_gain : 1_nm;
                        modal::dsp::num overtone = mode_idx_p1 * (1 + mode_idx * (inharmonicity));
                        modal::dsp::num mode_freq = freq / std::pow(overtone, exponent);
                        modal::dsp::num distance = (2.0_nm / std::pow((mode_idx + 1.0_nm), falloff)) * mode_gain;
                        modes[i].set_params(mode_freq, distance, distance * decay);
                    }
                    break;
                }
                case MiniModalFoldbackKind::Foldback: {
                    for (size_t i = 0; i < currentModes; i++) {
                        num mode_idx = static_cast<num>(i); // i
                        num mode_idx_p1 = mode_idx + 1; // k
                        num mode_gain = i % 2 == 1 ? even_gain : 1_nm;
                        modal::dsp::num overtone = mode_idx_p1 * (1 + mode_idx * (inharmonicity));
                        modal::dsp::num mode_freq = freq * std::pow(overtone, exponent);
                        // see https://www.desmos.com/calculator/2kbqwfyvjn
                        if (mode_freq > foldback.foldback_point) {
                            mode_freq = (2 * foldback.foldback_point) - mode_freq;
                        }
                        modal::dsp::num distance = (2.0_nm / std::pow((mode_idx + 1.0_nm), falloff)) * mode_gain;
                        modes[i].set_params(mode_freq, distance, distance * decay);
                    }
                    break;
                }
            }
            osc_exciter.set_freq(freq / exciter_rate);
        }

     private:
        void ping() {
            for (size_t i = 0; i < currentModes; i++) {
                modes[i].ping();
            }
        }
    };
}
