#include "geometry/ray.hpp"

Ray::Ray(
    const Eigen::Vector4d& origin,
    const Eigen::Vector4d& direction
)
    : origin_(origin), direction_(direction) {}

const Eigen::Vector4d& Ray::getOrigin() const {
    return origin_;
}

const Eigen::Vector4d& Ray::getDirection() const {
    return direction_;
}

Eigen::Vector4d Ray::at(double t) const {
    Eigen::Vector4d point = origin_ + direction_ * t;
    return point;
}