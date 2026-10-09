#include "geometry/transformation.hpp"

#include <utility>

#include "geometry/ray.hpp"

Transformation::Transformation(std::unique_ptr<Object> subject, const Eigen::Matrix4d& matrix)
    : subject_(std::move(subject)),
      matrix_(matrix),
      inverse_(matrix.inverse()),
      normalMatrix_(inverse_.transpose()) {}

const Object& Transformation::getSubject() const {
    return *subject_;
}

const Eigen::Matrix4d& Transformation::getMatrix() const {
    return matrix_;
}

const Eigen::Matrix4d& Transformation::getInverse() const {
    return inverse_;
}

bool Transformation::contains(const Eigen::Vector4d& x) const {
    return subject_->contains(inverse_ * x);
}

std::vector<Hit> Transformation::intersect(const Ray& ray) const {
    const Ray localRay(inverse_ * ray.getOrigin(), inverse_ * ray.getDirection());

    std::vector<Hit> hits = subject_->intersect(localRay);

    for (Hit& hit : hits) {
        hit.point = matrix_ * hit.point;

        Eigen::Vector4d normal = normalMatrix_ * hit.normal;
        normal.w() = 0.0;
        hit.normal = normal.normalized();
    }

    return hits;
}
