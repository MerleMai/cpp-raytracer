#pragma once

#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "geometry/object.hpp"

/**
 * @brief Base class for transformations applied to scene objects.
 *
 * A transformation exclusively owns one subject object and represents the
 * transformation as a 4x4 homogeneous matrix. Rays and points passed to
 * intersect()/contains() are mapped into the subject's local (untransformed)
 * coordinate space using the inverse matrix; the resulting hit points and
 * normals are mapped back into world space using the forward matrix and the
 * inverse-transpose "normal matrix" respectively. Concrete transformations
 * (Translation, Scaling, Rotation, ...) only need to construct the correct
 * matrix and pass it to this base class.
 */
class Transformation : public Object {
private:
    /// @brief Object to which the transformation is applied.
    std::unique_ptr<Object> subject_;

    /// @brief Forward transformation matrix, mapping local space to world space.
    Eigen::Matrix4d matrix_;

    /// @brief Inverse of the forward matrix, mapping world space to local space.
    Eigen::Matrix4d inverse_;

    /// @brief Inverse-transpose of the forward matrix, used to correct normals.
    Eigen::Matrix4d normalMatrix_;

protected:
    /**
     * @brief Constructs a transformation that owns a subject object.
     *
     * @param subject Object to which the transformation is applied.
     * @param matrix Forward 4x4 homogeneous transformation matrix.
     */
    Transformation(std::unique_ptr<Object> subject, const Eigen::Matrix4d& matrix);

public:
    /**
     * @brief Provides access to the transformed subject object.
     * 
     * @return Constant reference to the owned subject object.
     */
    const Object& getSubject() const;

    /**
     * @brief Provides access to the forward transformation matrix.
     *
     * @return Constant reference to the 4x4 matrix mapping local to world space.
     */
    const Eigen::Matrix4d& getMatrix() const;

    /**
     * @brief Provides access to the inverse transformation matrix.
     *
     * @return Constant reference to the 4x4 matrix mapping world to local space.
     */
    const Eigen::Matrix4d& getInverse() const;

    /**
     * @brief Checks if a point lies inside the transformed object volume.
     *
     * Maps the point into local space using the inverse matrix before
     * delegating the containment test to the subject object.
     *
     * @param x 4D point in world coordinates.
     *
     * @return True if the inversely transformed point lies inside the subject object.
     */
    bool contains(const Eigen::Vector4d& x) const override;

    /**
     * @brief Intersects a ray with the transformed object.
     *
     * Maps the ray into local space using the inverse matrix, evaluates hits
     * against the subject, and maps the resulting hit points back to world
     * space using the forward matrix and hit normals using the normal matrix.
     *
     * @param ray The ray to test in world coordinates.
     *
     * @return Vector of valid Hit instances mapped back to world space.
     */
    std::vector<Hit> intersect(const Ray& ray) const override;
};
