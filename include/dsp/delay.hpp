// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <vector>

#include <dsp/dsp.hpp>
#include <dsp/bonus.hpp>

namespace modal::dsp {
    class delay_line {
        public:
            delay_line(num sr, num mt);

            void push_sample(num s);

            [[nodiscard]] num fetch_sample_s(num time_backwards) const;

            // with -O1 and better this optimises smaller than `(i % len + len) % len` on x64 and ARM - https://godbolt.org/z/aj1GsTovM
            [[nodiscard]] num fetch_sample_sm(int samps_backwards) const;

            void set_sample_rate(num sr);

            void set_max_time(num new_max_time);

            [[nodiscard]] num get_max_time() const;

            [[nodiscard]] unsigned long get_max_samples() const;

        private:
            void resize_buffer();

            num sample_rate = 0;
            num max_time = 0;
            unsigned long idx = 0;
            std::vector<num> buffer;
    };
}