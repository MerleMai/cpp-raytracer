#pragma once
#include <Eigen/Core>

/**
 * @brief Represents a point light source in the scene.
 * 
 * A light source is defined by its position in homogeneous coordinates 
 * and its RGB intensity used for illumination calculations.
 */
class LightSource {
private:
    /// @brief Position of the light source in homogeneous coordinates.
    Eigen::Vector4d position_;

    /// @brief RGB intensity coefficients of emitted light. 
    Eigen::Vector3d intensity_;

public:
    /**
     * @brief Constructs a point light source with position and intensity parameters.
     * 
     * @param position Position of the light source in homogeneous coordinates.
     * @param intensity RGB light intensity vector. 
     */
    LightSource(
        const Eigen::Vector4d& position,
        const Eigen::Vector3d& intensity
    );

    /**
     * @brief Provides the world-space position of the light source.
     * 
     * @return Constant reference to the 4D position vector in homogeneous coordinates.
     */
    const Eigen::Vector4d& getPosition() const;

    /**
     * @brief Provides the RGB intensity values emitted by the light source.
     * 
     * @return Constant reference to the 3D RGB light intensity vector.
     */
    const Eigen::Vector3d& getIntensity() const;    
};
