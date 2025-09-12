#include "catch2/benchmark/catch_benchmark.hpp"

static unsigned int factorial(const unsigned int n) {
    return n > 1 ? factorial(n - 1) * n : 1;
}

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Factorials are computed", "[factorial]") {
    REQUIRE(factorial(0) == 1);
    REQUIRE(factorial(1) == 1);
    REQUIRE(factorial(2) == 2);
    REQUIRE(factorial(3) == 6);
    REQUIRE(factorial(10) == 3628800);
    BENCHMARK("Fibonacci 20") {
        return factorial(20);
    };


}