#include "scene/screen.hpp"

#include <cmath>

Screen::Screen(
    int dpi,
    const Eigen::Vector2d& dimensions,
    const Eigen::Vector4d& position,
    const Eigen::Vector4d& observer
) : dpi_(dpi), dimensions_(dimensions), position_(position), observer_(observer) {}

int Screen::getDpi() const {
    return dpi_;
}

const Eigen::Vector2d& Screen::getDimensions() const {
    return dimensions_;
}

const Eigen::Vector4d& Screen::getPosition() const {
    return position_;
}

const Eigen::Vector4d& Screen::getObserver() const {
    return observer_;
}

int Screen::getWidth() const {
    double widthDouble = dpi_ * dimensions_.x();
    long widthLong = std::lround(widthDouble);
    int width = static_cast<int>(widthLong);
    return width;
}

int Screen::getHeight() const {
    double heightDouble = dpi_ * dimensions_.y();
    long heightLong = std::lround(heightDouble);
    int height = static_cast<int>(heightLong);
    return height;
}

Ray Screen::rayFor(int column, int row, double offsetX, double offsetY) const {
    const double step = 1.0 / static_cast<double>(dpi_);
    const double x = position_.x() + (column + offsetX) * step;
    const double y = position_.y() + (row + offsetY) * step;

    const Eigen::Vector4d pixel(x, y, position_.z(), 1.0);
    Eigen::Vector4d direction = pixel - observer_;
    direction.head<3>().normalize();

    return Ray(observer_, direction);
}
