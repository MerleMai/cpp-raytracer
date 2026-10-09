#pragma once

#include <memory>
#include <vector>

#include <Eigen/Core>

#include "geometry/objectCombination.hpp"

/**
 * @brief Represents the logical Constructive Solid Geometry (CSG) union of multiple objects.
 * 
 * Defines the combined spatial volume contained within at least one of its child objects.
 */
class Union : public ObjectCombination {
public:
    /**
     * @brief Constructs a CSG union combination from its child objects.
     *
     * @param children Objects participating in the union.
     */
    explicit Union(
        std::vector<std::unique_ptr<Object>> children
    );

    /**
     * @brief Checks if a point lies inside any of the child objects in the union.
     * 
     * @param x 4D point in homogeneous coordinates.
     * @return True if at least one child object contains the point, false otherwise.
     */
    bool contains(const Eigen::Vector4d& x) const override;

    /**
     * @brief Computes all visible external surface intersections between a ray and the CSG union volume.
     * 
     * Intersections that lie inside any other participating child object are discarded
     * to eliminate internal boundary surfaces, and the remaining hits are sorted along the ray.
     * 
     * @param ray The ray to intersect with the union volume.
     * @return Sorted vector of valid outer surface Hit structures along the ray.
     */
    std::vector<Hit> intersect(const Ray& ray) const override;
};
