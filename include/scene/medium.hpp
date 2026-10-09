#pragma once

#include <Eigen/Core>

/**
 * @brief Stores the global optical properties of the surrounding scene medium.
 *
 * The medium defines the ambient RGB light intensity present throughout the scene, the refractive
 * index of the surrounding environment (e.g., air or vacuum) and the maximum recursion depth used when
 * tracing reflected and refracted rays. 
 */
class Medium {
private:
    /// @brief Ambient light intensity of the scene as an RGB vector
    Eigen::Vector3d ambient_;

    /// @brief Refractive index of the surrounding medium
    double index_;

    /// @brief Maximum allowed recursion depth for reflected and refracted rays
    int recursion_;

public:
    /**
     * @brief Constructs a medium instance from its optical parameters.
     * 
     * @param ambient Ambient RGB light intensity of the scene.
     * @param index Refractive index of the surrounding medium.
     * @param recursion Maximum recursion depth for ray reflection and refraction. 
     */
    Medium(
        const Eigen::Vector3d& ambient,
        double index,
        int recursion
    );

    /**
     * @brief Provides the ambient RGB light intensity of the environment.
     * 
     * @return Constant reference to the 3D RGB ambient intensity vector.
     */
    const Eigen::Vector3d& getAmbient() const;

    /**
     * @brief Provides the refractive index of the surrounding medium.
     * 
     * @return Refractive index used for refraction calculations. 
     */
    double getIndex() const; 

    /**
     * @brief Provides the maximum recursion depth limit for secondary rays.
     * 
     * @return Maximum depth limit for recursive raytracing steps.
     */
    int getRecursion() const;
};
