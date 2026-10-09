#pragma once

#include <memory>
#include <Eigen/Core>

#include "geometry/transformation.hpp"

/**
 * @brief Represents an non-uniform 3D scaling transformation applied to a child object.
 * 
 * Maps world-space rays into the scaled object coordinate space, evaluates surface intersections,
 * and corrects surface normal vectors using inverse-transpose scale factors.
 * Internally represented as a 4x4 homogeneous scaling matrix (see Transformation).
 */
class Scaling : public Transformation {
private:
    /// @brief Scaling factors along the x, y, and z axes.
    Eigen::Vector3d factors_;

public:
   /**
     * @brief Constructs a scaling transformation with specific axis scale factors.
     *
     * @param factors 3D vector containing non-zero scaling factors along x, y, and z axes.
     * @param subject Object to be scaled.
     */
    Scaling(
        const Eigen::Vector3d& factors,
        std::unique_ptr<Object> subject
    );

    /**
     * @brief Provides access to the scaling factors.
     * 
     * @return Constant reference to the 3D vector of scale factors.
     */
    const Eigen::Vector3d& getFactors() const;
};
