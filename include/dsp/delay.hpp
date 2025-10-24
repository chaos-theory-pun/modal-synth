// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <vector>

#include <dsp/dsp.hpp>
#include <dsp/bonus.hpp>

namespace modal::dsp {
    class delay_line {
        public:
            delay_line(const num sr, const num mt) : sample_rate{sr}, max_time {mt} {
                buffer.resize(sr * max_time, 0.0);
            }

            void push_sample(num s) {
                buffer[idx] = s;
                idx = (idx + 1) % static_cast<int>(buffer.size());
            }

            num fetch_sample_s(const num time_backwards) {
                const num samps_back = time_backwards * sample_rate;
                const int idx_1 = static_cast<int>(samps_back);
                const int idx_2 = idx_1 + 1;
                const num frac = samps_back - static_cast<int>(samps_back);

                return dsp::bonus::lerp(fetch_sample_sm(idx_1), fetch_sample_sm(idx_2), frac);
            }

            // with -O1 and better this optimises smaller than `(i % len + len) % len` on x64 and ARM - https://godbolt.org/z/aj1GsTovM
            num fetch_sample_sm(const int samps_backwards) {
                long get_idx = idx - samps_backwards;
                if (get_idx < 0) {
                    get_idx += static_cast<long>(get_max_samples());
                }
                return buffer[get_idx];
            }

            void set_sample_rate(const num sr) {
                sample_rate = sr;
                buffer.resize(sr * max_time, 0.0);
            }

            void set_max_time(const num new_max_time) {
                max_time = new_max_time;
                buffer.resize(sample_rate * max_time, 0.0);
            }

            [[nodiscard]] num get_max_time() const {
                return static_cast<num>(buffer.size()) / static_cast<num>(sample_rate);
            }

            [[nodiscard]] unsigned long get_max_samples() const {
                return buffer.size();
            }

        private:
            num sample_rate = 0;
            num max_time = 0;
            int idx = 0;
            std::vector<num> buffer;
    };
}