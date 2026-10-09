#include "geometry/rotation.hpp"

#include <cmath>
#include <utility>

namespace {

Eigen::Matrix4d rotationMatrix(double angleDegrees, RotationAxis axis) {
    const double radians = angleDegrees * std::acos(-1.0) / 180.0;
    const double cosine = std::cos(radians);
    const double sine = std::sin(radians);

    Eigen::Matrix4d matrix = Eigen::Matrix4d::Identity();

    switch (axis) {
        case RotationAxis::X:
            matrix(1, 1) = cosine;
            matrix(1, 2) = -sine;
            matrix(2, 1) = sine;
            matrix(2, 2) = cosine;
            break;
        case RotationAxis::Y:
            matrix(0, 0) = cosine;
            matrix(0, 2) = sine;
            matrix(2, 0) = -sine;
            matrix(2, 2) = cosine;
            break;
        case RotationAxis::Z:
            matrix(0, 0) = cosine;
            matrix(0, 1) = -sine;
            matrix(1, 0) = sine;
            matrix(1, 1) = cosine;
            break;
    }

    return matrix;
}

}

Rotation::Rotation(
    double angleDegrees,
    RotationAxis axis,
    std::unique_ptr<Object> subject
)
    : Transformation(std::move(subject), rotationMatrix(angleDegrees, axis)),
      angleDegrees_(angleDegrees),
      axis_(axis) {}

double Rotation::getAngleDegrees() const {
    return angleDegrees_;
}

RotationAxis Rotation::getAxis() const {
    return axis_;
}
