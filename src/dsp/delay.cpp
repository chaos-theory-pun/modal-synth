// SPDX-License-Identifier: GPL-3.0-or-later

#include <dsp/delay.hpp>

modal::dsp::delay_line::delay_line(const num sr, const num mt): sample_rate{sr}, max_time {mt} {
    resize_buffer();
}

void modal::dsp::delay_line::push_sample(num s) {
    buffer[idx] = s;
    idx = (idx + 1) % buffer.size();
}

modal::dsp::num modal::dsp::delay_line::fetch_sample_s(const num time_backwards) const {
    const num samps_back = time_backwards * sample_rate;
    const int idx_1 = static_cast<int>(samps_back);
    const int idx_2 = idx_1 + 1;
    const num frac = static_cast<num>(samps_back - static_cast<int>(samps_back));

    return dsp::bonus::lerp(fetch_sample_sm(idx_1), fetch_sample_sm(idx_2), frac);
}

modal::dsp::num modal::dsp::delay_line::fetch_sample_sm(const int samps_backwards) const {
    long get_idx = static_cast<long>(idx) - samps_backwards;
    if (get_idx < 0) {
        get_idx += static_cast<long>(get_max_samples());
    }
    return buffer[static_cast<unsigned long>(get_idx)];
}

void modal::dsp::delay_line::set_sample_rate(const num sr) {
    sample_rate = sr;
    resize_buffer();
}

void modal::dsp::delay_line::set_max_time(const num new_max_time) {
    max_time = new_max_time;
    resize_buffer();
}

modal::dsp::num modal::dsp::delay_line::get_max_time() const {
    return static_cast<num>(buffer.size()) / static_cast<num>(sample_rate);
}

unsigned long modal::dsp::delay_line::get_max_samples() const {
    return buffer.size();
}

void modal::dsp::delay_line::resize_buffer() {
    buffer.resize(static_cast<unsigned long>(sample_rate * max_time), 0.0);
}
