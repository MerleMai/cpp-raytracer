#pragma once

/**
 * @brief Epsilon threshold used for floating-point tolerance in geometric calculations.
 * 
 * Prevents numerical precision issues such as self-intersection ("shadow acne")
 * during ray-object intersection checks.
 */
constexpr double GEOMETRY_EPSILON = 1e-6;
