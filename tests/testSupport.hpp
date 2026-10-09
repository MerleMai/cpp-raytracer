#pragma once

/**
 * @file testSupport.hpp
 * @brief Minimal shared helpers for the automated test executables.
 *
 * The tests deliberately avoid assert(): assertions are compiled out in
 * Release builds (NDEBUG), which would silently turn every check into a
 * no-op. expect() throws instead, so a failing check is reported in every
 * build type and the test executable returns a non-zero exit code to CTest.
 */

#include <cmath>
#include <stdexcept>
#include <string>

/**
 * @brief Reports a failed test expectation without relying on assert.
 *
 * @param condition Result of the checked expression.
 * @param expression Text representation of the checked expression.
 * @param line Source line at which the expectation was evaluated.
 * @throws std::runtime_error If the checked expression is false.
 */
inline void expect(
    bool condition,
    const char* expression,
    int line
) {
    if (!condition) {
        throw std::runtime_error(
            "Test expectation failed at line " +
            std::to_string(line) +
            ": " +
            expression
        );
    }
}

/**
 * @brief Checks a condition and records its source text and line on failure.
 */
#define EXPECT(condition) \
    expect((condition), #condition, __LINE__)

/**
 * @brief Compares two floating-point values using a small absolute tolerance.
 *
 * Exact comparison with == is unsuitable for results of floating-point
 * geometry, which carry rounding errors of a few ulps.
 *
 * @param a First value.
 * @param b Second value.
 * @return True if the values differ by less than 1e-9.
 */
inline bool nearlyEqual(double a, double b) {
    constexpr double tolerance = 1e-9;
    return std::abs(a - b) < tolerance;
}
