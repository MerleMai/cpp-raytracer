#include "geometry/scaling.hpp"

#include <utility>

namespace {

Eigen::Matrix4d scalingMatrix(const Eigen::Vector3d& factors) {
    Eigen::Matrix4d matrix = Eigen::Matrix4d::Identity();
    matrix(0, 0) = factors.x();
    matrix(1, 1) = factors.y();
    matrix(2, 2) = factors.z();
    return matrix;
}

}

Scaling::Scaling(
    const Eigen::Vector3d& factors,
    std::unique_ptr<Object> subject
)
    : Transformation(std::move(subject), scalingMatrix(factors)),
      factors_(factors) {}

const Eigen::Vector3d& Scaling::getFactors() const {
    return factors_;
}
