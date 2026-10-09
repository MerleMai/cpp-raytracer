#include "geometry/sphere.hpp"

#include <cmath>

#include "geometry/constants.hpp"

Sphere::Sphere(
    const Eigen::Vector4d& position,
    double radius,
    const Material& material,
    double index
)
    : PrimitiveObject(material, index),
      position_(position),
      radius_(radius) {}

const Eigen::Vector4d& Sphere::getPosition() const {
    return position_;
}

double Sphere::getRadius() const {
    return radius_;
}

bool Sphere::contains(const Eigen::Vector4d& point) const {
    return (position_ - point).norm() <= radius_;
}

std::vector<Hit> Sphere::intersect(const Ray& ray) const {
    const Eigen::Vector3d direction = ray.getDirection().head<3>();
    const Eigen::Vector3d originToCenter = ray.getOrigin().head<3>() - position_.head<3>();

    const double a = direction.dot(direction);
    const double b = originToCenter.dot(direction);
    const double c = originToCenter.dot(originToCenter) - radius_ * radius_;
    const double discriminant = b * b - a * c;

    if (discriminant < 0.0) {
        return {};
    }

    const double squareRoot = std::sqrt(discriminant);
    std::vector<Hit> hits;

    for (double t : {(-b - squareRoot) / a, (-b + squareRoot) / a}) {
        if (t <= GEOMETRY_EPSILON) {
            continue;
        }

        Hit hit;
        hit.t = t;
        hit.point = ray.at(t);
        hit.normal = hit.point - position_;
        hit.normal.w() = 0.0;
        hit.normal.head<3>().normalize();
        hit.primitive = this;
        hit.entering = hit.normal.head<3>().dot(direction) < 0.0;
        hits.push_back(hit);
    }

    return hits;
}
