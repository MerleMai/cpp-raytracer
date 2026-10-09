#include "geometry/translation.hpp"

#include <utility>

namespace {

Eigen::Matrix4d translationMatrix(const Eigen::Vector3d& factors) {
    Eigen::Matrix4d matrix = Eigen::Matrix4d::Identity();
    matrix(0, 3) = factors.x();
    matrix(1, 3) = factors.y();
    matrix(2, 3) = factors.z();
    return matrix;
}

}

Translation::Translation(
    const Eigen::Vector3d& factors,
    std::unique_ptr<Object> subject
)
    : Transformation(std::move(subject), translationMatrix(factors)),
      factors_(factors) {}

const Eigen::Vector3d& Translation::getFactors() const {
    return factors_;
}
