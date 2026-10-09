#pragma once

#include <memory>
#include <vector>

#include <Eigen/Core>

#include "geometry/objectCombination.hpp"

/**
 * @brief Represents the logical Constructive Solid Geometry (CSG) intersection of multiple objects.
 * 
 * Defines the spatial volume contained simultaneously within all child objects.
 */
class Intersection : public ObjectCombination {
public:
    /**
     * @brief Constructs an intersection CSG combination from its child objects.
     *
     * @param children Collection of objects participating in the intersection.
     */
    explicit Intersection(
        std::vector<std::unique_ptr<Object>> children
    );

    /**
     * @brief Checks if a given point lies within all child objects of the intersection.
     * 
     * @param x 4D point in homogeneous coordinates.
     * 
     * @return True if every child object contains the point, false otherwise.
     */
    bool contains(const Eigen::Vector4d& x) const override;

    /**
     * @brief Computes all intersection points between a ray and the CSG intersection volume.
     * 
     * Intersection hits are filtered to keep only those points that simultaneously lie
     * inside all other participating child objects.
     * 
     * @param ray The ray to intersect with the combined volume.
     * 
     * @return Sorted vector of valid intersection hits along the ray path.
     */
    std::vector<Hit> intersect(const Ray& ray) const override;
};
