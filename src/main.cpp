#include <exception>
#include <iostream>

#include "io/imageWriter.hpp"
#include "io/sceneParser.hpp"
#include "render/renderer.hpp"

/**
 * @file main.cpp
 * @brief Command-line entry point of the raytracer.
 */

/**
 * @brief Renders one JSON scene file into one PNG image.
 *
 * Usage: `raytracer <scene.json> <output.png>`.
 *
 * This is the application boundary: every std::exception raised while
 * loading, rendering, or writing (for example the std::runtime_error thrown
 * by SceneParser::load() for invalid input) is caught here and printed as an
 * error message instead of terminating the program.
 *
 * @param argc Number of command-line arguments; must be exactly 3.
 * @param argv Program name, scene file path, and output image path.
 * @return 0 on success, 1 on wrong usage or any error.
 */
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "usage: raytracer <scene.json> <output.png>\n";
        return 1;
    }

    try {
        const Scene scene = SceneParser::load(argv[1]);
        const Renderer renderer(scene);
        ImageWriter::save(renderer.render(), scene.getScreen(), argv[2]);
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
