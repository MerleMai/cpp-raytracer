#include "geometry/material.hpp"

Material::Material(
    const Eigen::Vector3d& ambient,
    const Eigen::Vector3d& diffuse,
    const Eigen::Vector3d& specular,
    const Eigen::Vector3d& reflected,
    const Eigen::Vector3d& refracted,
    double shininess
) 
    : ambient_(ambient),
      diffuse_(diffuse),
      specular_(specular),
      reflected_(reflected),
      refracted_(refracted),
      shininess_(shininess) {}

Material::Material()
    : ambient_(Eigen::Vector3d::Zero()),
      diffuse_(Eigen::Vector3d::Zero()),
      specular_(Eigen::Vector3d::Zero()),
      reflected_(Eigen::Vector3d::Zero()),
      refracted_(Eigen::Vector3d::Zero()),
      shininess_(1.0) {}

const Eigen::Vector3d& Material::getAmbient() const {
    return ambient_;
}

const Eigen::Vector3d& Material::getDiffuse() const {
    return diffuse_;
}

const Eigen::Vector3d& Material::getSpecular() const {
    return specular_;
}

const Eigen::Vector3d& Material::getReflected() const {
    return reflected_;
}

const Eigen::Vector3d& Material::getRefracted() const {
    return refracted_;
}

double Material::getShininess() const {
    return shininess_;
}