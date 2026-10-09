#pragma once

#include <memory>

#include <Eigen/Core>

#include "geometry/transformation.hpp"

/**
 * @brief Represents a three-dimensional translation applied to an object.
 * 
 * Shifts the position of an object in world space by offset vectors along the x, y, and z axes.
 * Internally represented as a 4x4 homogeneous translation matrix (see Transformation).
 */
class Translation : public Transformation {
private:
    /// @brief Translation factors along the x, y, and z axes.
    Eigen::Vector3d factors_;

public:
    /**
     * @brief Constructs a translation transformation.
     *
     * @param factors Translation factors along x, y, and z.
     * @param subject Object to be translated.
     */
    Translation(
        const Eigen::Vector3d& factors,
        std::unique_ptr<Object> subject
    );

    /**
     * @brief Provides access to the translation offset factors.
     * 
     * @return Constant reference to the 3D translation vector.
     */
    const Eigen::Vector3d& getFactors() const;
};
