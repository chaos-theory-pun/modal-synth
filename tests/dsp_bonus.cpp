#include <dsp/bonus.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Midi note to frequency (Hz)", "[dsp][midi2freq]") {
    using namespace modal::dsp::bonus;
    REQUIRE(midi2freq(69) == 440.f);
}