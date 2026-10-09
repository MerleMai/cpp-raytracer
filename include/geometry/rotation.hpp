#pragma once

#include <memory>
#include <vector>

#include <Eigen/Core>

#include "geometry/transformation.hpp"

/**
 * @brief Axis around which a 3D rotation transformation is performed.
 */
enum class RotationAxis {
    X,
    Y,
    Z
};

/**
 * @brief Represents an affine rotation transformation applied to a child object.
 * 
 * Transforms incoming rays into the unrotated local coordinate system of the subject,
 * evaluates intersections, and transforms resulting points and normals back to world space.
 * Internally represented as a 4x4 homogeneous rotation matrix (see Transformation).
 */
class Rotation : public Transformation {
private:
    /// @brief Rotation angle in degrees.
    double angleDegrees_;

    /// @brief Global axis around which the object is rotated.
    RotationAxis axis_;

public:
    /**
     * @brief Constructs a rotation transformation for a subject object.
     *
     * @param angleDegrees Rotation angle in degrees.
     * @param axis Axis around which the rotation is performed.
     * @param subject Object to be rotated.
     */
    Rotation(
        double angleDegrees,
        RotationAxis axis,
        std::unique_ptr<Object> subject
    );

   /**
     * @brief Returns the rotation angle in degrees.
     * 
     * @return Rotation angle in degrees.
     */
    double getAngleDegrees() const;

   /**
     * @brief Returns the axis of rotation.
     * 
     * @return RotationAxis enum value representing the axis.
     */
    RotationAxis getAxis() const;
};
