#pragma once

#include "geometry/material.hpp"
#include "geometry/object.hpp"

/**
 * @brief Base class for primitive geometric objects.
 *
 * Primitive objects define their own optical material and refractive
 * index. Transformations and object combinations do not store these
 * properties themselves; they use the properties of the primitive
 * object at the relevant intersection point.
 */
class PrimitiveObject : public Object {
private:
    /// @brief Optical material coefficients of the primitive.
    Material material_;

    /// @brief Refractive index of the primitive object.
    double index_;

protected:
    /**
     * @brief Constructs a primitive object from its optical properties.
     *
     * @param material Optical material coefficients of the primitive.
     * @param index Refractive index of the primitive.
     */
    PrimitiveObject(
        const Material& material,
        double index
    );

public:
   /**
     * @brief Provides access to the material coefficients of the primitive object.
     * 
     * @return Constant reference to the assigned Material instance.
     */
    const Material& getMaterial() const;

   /**
     * @brief Returns the refractive index of the primitive object.
     * 
     * @return Refractive index value.
     */
    double getIndex() const;
};
