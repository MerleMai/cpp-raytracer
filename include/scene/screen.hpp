#pragma once

#include <Eigen/Core>

#include "geometry/ray.hpp"

/**
 * @brief Represents the camera screen and observer position in 3D space.
 * 
 * A screen consists of a rectangular surface located in an x-y plane and an
 * observer looking through that surface. The screen resolution is determined
 * by its physical dimensions and the number of pixels per length unit (dpi).
 *
 * Geometric positions are stored in homogeneous coordinates.
 * Position and observer must have a fourth coordinate equal to 1.
 */
class Screen {
private:
    /// @brief Number of pixels per length unit in both screen directions
    int dpi_;

    /// @brief Physical screen dimensions (width, height)
    Eigen::Vector2d dimensions_;

    /// @brief Bottom-left point of the screen in homogeneous coordinates
    Eigen::Vector4d position_;

    /// @brief Observer position in homogeneous coordinates
    Eigen::Vector4d observer_;

public:
    /**
     * @brief Constructs a screen from its geometric parameters.
     * 
     * @param dpi Number of pixels per length unit
     * @param dimensions Physical width and height of the screen
     * @param position Bottom-left screen position in homogeneous coordinates 
     * @param observer Observer position in homogeneous coordinates
     */
    Screen(
        int dpi,
        const Eigen::Vector2d& dimensions,
        const Eigen::Vector4d& position,
        const Eigen::Vector4d& observer
    );

    /**
     * @brief Provides the screen resolution density.
     * 
     * @return Number of pixels per physical length unit.
     */
    int getDpi() const;

    /**
     * @brief Provides the physical dimensions of the screen.
     * 
     * @return Constant reference to the 2D vector containing physical height and width.
     */
    const Eigen::Vector2d& getDimensions() const;

    /**
     * @brief Provides the world position of the screen's bottom-left corner.
     * 
     * @return Constant reference to the 4D position vector in homogeneous coordinates.
     */
    const Eigen::Vector4d& getPosition() const;

    /**
     * @brief Provides the location of the observer.
     * 
     * @return Constant reference to the 4D observer position vector in homogeneous coordinates.
     */
    const Eigen::Vector4d& getObserver() const;

    /**
     * @brief Returns the total screen width in pixels.
     * 
     * Calculates the width by multiplying the physical x-dimension by the dpi
     * resolution and rounding to the nearest integer.
     * 
     * @return Screen width in pixels.
     */
    int getWidth() const;

    /**
     * @brief Returns the total screen height in pixels.
     * 
     * Calculates the height by multiplying the physical y-dimension by the dpi
     * resolution and rounding to the nearest integer.
     * 
     * @return Screen height in pixels.
     */
    int getHeight() const;

    /**
     * @brief Generates a ray passing from the observer through a specific pixel location.
     * 
     * Computes the normalized ray starting at the observer position and directed 
     * towards the specified pixel coordinate on the screen, including optional sub-pixel offsets.
     * 
     * @param column Zero-based pixel column index.
     * @param row Zero-based pixel row index.
     * @param offsetX Sub-pixel offset along the x-axis.
     * @param offsetY Sub-pixel offset along the y-axis.
     * 
     * @return Normalized ray from the observer through the screen pixel.
     */
    Ray rayFor(int column, int row, double offsetX = 0.5, double offsetY = 0.5) const;
};
