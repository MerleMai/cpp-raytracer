#include "geometry/halfSpace.hpp"

#include <cmath>

#include "geometry/constants.hpp"

HalfSpace::HalfSpace(
    const Eigen::Vector4d& position,
    const Eigen::Vector4d& normal,
    const Material& material,
    double index
)
    : PrimitiveObject(material, index),
      position_(position),
      normal_(normal) {}

const Eigen::Vector4d& HalfSpace::getPosition() const {
    return position_;
}

const Eigen::Vector4d& HalfSpace::getNormal() const {
    return normal_;
}

bool HalfSpace::contains(const Eigen::Vector4d& x) const {
    return normal_.dot(x - position_) <= 0;
}

std::vector<Hit> HalfSpace::intersect(const Ray& ray) const {
    const double denominator = normal_.dot(ray.getDirection());

    if (std::abs(denominator) < 1e-12) {
        return {};
    }

    const double t = normal_.dot(position_ - ray.getOrigin()) / denominator;

    if (t <= GEOMETRY_EPSILON) {
        return {};
    }

    Hit hit;
    hit.t = t;
    hit.point = ray.at(t);
    hit.normal = normal_;
    hit.primitive = this;
    hit.entering = denominator < 0.0;

    return {hit};
}
