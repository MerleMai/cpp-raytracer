#include "io/imageWriter.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>

#include <opencv2/opencv.hpp>

namespace {

unsigned char toByte(double value) {
    return static_cast<unsigned char>(std::lround(std::clamp(value, 0.0, 1.0) * 255.0));
}

}

void ImageWriter::save(
    const std::vector<Eigen::Vector3d>& pixels,
    const Screen& screen,
    const std::string& filename
) {
    const int width = screen.getWidth();
    const int height = screen.getHeight();

    cv::Mat image(height, width, CV_8UC3);

    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            const Eigen::Vector3d& color = pixels[static_cast<std::size_t>(row) * width + column];
            image.at<cv::Vec3b>(height - 1 - row, column) = cv::Vec3b(
                toByte(color.z()),
                toByte(color.y()),
                toByte(color.x())
            );
        }
    }

    if (!cv::imwrite(filename, image)) {
        throw std::runtime_error("Could not write image file: " + filename);
    }
}
