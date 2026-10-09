#pragma once

#include <Eigen/Core>

/**
 * @brief Blends two RGB color vectors using a screen blending mode.
 * 
 * Combines two color vectors using the formula 1 - (1 - first) * (1 - second)
 * component-wise. This ensures that combined light intensities accumulate smoothly
 * without exceeding maximum brightness values.
 * 
 * @param first First RGB color vector.
 * @param second Second RGB color vector.
 * 
 * @return Blended 3D RGB color vector.
 */
inline Eigen::Vector3d combine(const Eigen::Vector3d& first, const Eigen::Vector3d& second) {
    const Eigen::Vector3d one = Eigen::Vector3d::Ones();
    return one - (one - first).cwiseProduct(one - second);
}
