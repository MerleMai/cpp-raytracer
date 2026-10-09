#include "scene/medium.hpp"

Medium::Medium(
    const Eigen::Vector3d& ambient,
    double index,
    int recursion
) : ambient_(ambient), index_(index), recursion_(recursion) {}

const Eigen::Vector3d& Medium::getAmbient() const {
    return ambient_;
}

double Medium::getIndex() const {
    return index_;
}

int Medium::getRecursion() const {
    return recursion_;
}
