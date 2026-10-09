#pragma once

#include <string>
#include <vector>

#include <Eigen/Core>

#include "scene/screen.hpp"

/**
 * @brief Exports rendered pixel buffer data into an image file using OpenCV.
 * 
 * Maps RGB color vectors to 8-bit BGR color channels, accounts for 
 * coordinate origin flipping (converting 3D world/screen coordinates to 
 * image pixel rows), and saves the image to disk.
 */
class ImageWriter {
public:
    /**
     * @brief Saves a buffer of RGB pixel colors to a file.
     * 
     * Converts normalized 3D RGB floating-point vectors into 8-bit BGR values,
     * arranges them according to screen resolution, and writes the output file.
     * 
     * @param pixels Vector of 3D RGB color vectors in row-major order.
     * @param screen Screen instance supplying width and height resolution.
     * @param filename Target file path for saving the rendered image (e.g., .png).
     * 
     * @throws std::runtime_error If writing the image file fails.
     */
    static void save(
        const std::vector<Eigen::Vector3d>& pixels,
        const Screen& screen,
        const std::string& filename
    );
};
