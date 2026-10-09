#pragma once

#include <Eigen/Core>

/**
 * @brief Stores the optical material coefficients of a primitive object.
 *
 * A material defines the RGB coefficients used for ambient, diffuse,
 * specular, reflected, and refracted light contributions, together
 * with the shininess coefficient used by the Phong reflection model.
 */
class Material {
private:
    /// @brief RGB coefficient for ambient illumination
    Eigen::Vector3d ambient_;

    /// @brief RGB coefficient for diffuse illumination
    Eigen::Vector3d diffuse_;

    /// @brief RGB coefficient for specular illumination
    Eigen::Vector3d specular_;

    /// @brief RGB coefficient applied to reflected light
    Eigen::Vector3d reflected_;

    /// @brief RGB coefficient applied to refracted light
    Eigen::Vector3d refracted_;

    /// @brief Shininess coefficient used for specular illumination
    double shininess_;

public:
    /**
     * @brief Constructs a material from its optical coefficients.
     *
     * @param ambient Ambient RGB coefficient.
     * @param diffuse Diffuse RGB coefficient.
     * @param specular Specular RGB coefficient.
     * @param reflected Reflected-light RGB coefficient.
     * @param refracted Refracted-light RGB coefficient.
     * @param shininess Shininess coefficient.
     */
    Material(
        const Eigen::Vector3d& ambient,
        const Eigen::Vector3d& diffuse,
        const Eigen::Vector3d& specular,
        const Eigen::Vector3d& reflected,
        const Eigen::Vector3d& refracted,
        double shininess
    );

    /**
     * @brief Constructs the default black material.
     *
     * All RGB coefficients are initialized to zero and shininess is
     * initialized to 1, matching the default values of the scene
     * input format.
     */
    Material();

   /**
     * @brief Provides access to the ambient RGB reflection coefficient.
     * 
     * @return Constant reference to the ambient RGB vector.
     */
    const Eigen::Vector3d& getAmbient() const;

   /**
     * @brief Provides access to the diffuse RGB reflection coefficient.
     * 
     * @return Constant reference to the diffuse RGB vector.
     */
    const Eigen::Vector3d& getDiffuse() const;

    /**
     * @brief Provides access to the specular RGB reflection coefficient.
     * 
     * @return Constant reference to the specular RGB vector.
     */
    const Eigen::Vector3d& getSpecular() const;
/**
     * @brief Provides access to the reflected light RGB attenuation coefficient.
     * 
     * @return Constant reference to the reflected light RGB vector.
     */
    const Eigen::Vector3d& getReflected() const;

    /**
     * @brief Provides access to the refracted light RGB transmission coefficient.
     * 
     * @return Constant reference to the refracted light RGB vector.
     */
    const Eigen::Vector3d& getRefracted() const;

    /**
     * @brief Returns the specular shininess exponent.
     * 
     * @return Shininess exponent value.
     */
    double getShininess() const;
};
