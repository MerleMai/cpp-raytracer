#include "scene/lightSource.hpp"

LightSource::LightSource(
    const Eigen::Vector4d& position,
    const Eigen::Vector3d& intensity
) : position_(position), intensity_(intensity) {}

const Eigen::Vector4d& LightSource::getPosition() const {
    return position_;
}

const Eigen::Vector3d& LightSource::getIntensity() const {
    return intensity_;
}
