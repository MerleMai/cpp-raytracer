#include "geometry/primitiveObject.hpp"

PrimitiveObject::PrimitiveObject(
    const Material& material,
    double index
)
    : material_(material),
      index_(index) {}

const Material& PrimitiveObject::getMaterial() const {
    return material_;
}

double PrimitiveObject::getIndex() const {
    return index_;
}