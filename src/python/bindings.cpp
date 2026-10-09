#include <pybind11/pybind11.h>

#include "io/imageWriter.hpp"
#include "io/sceneParser.hpp"
#include "render/renderer.hpp"

namespace py = pybind11;

/**
 * @file bindings.cpp
 * @brief Optional pybind11 module `raytracer_py` exposing the renderer to Python.
 */

/**
 * @brief Loads a JSON scene, renders it, and writes the image to disk.
 *
 * Performs exactly the same steps as the command-line program
 * (SceneParser::load() -> Renderer::render() -> ImageWriter::save()), so
 * both entry points always produce identical images. Exceptions are not
 * caught here: pybind11 translates std::runtime_error into a Python
 * RuntimeError that the caller can handle.
 *
 * @param scenePath Path to the JSON scene file.
 * @param outputPath Path of the image file to write (e.g. `.png`).
 * @throws std::runtime_error If the scene is invalid or the image cannot be written.
 */
void renderScene(const std::string& scenePath, const std::string& outputPath) {
    const Scene scene = SceneParser::load(scenePath);
    const Renderer renderer(scene);
    ImageWriter::save(renderer.render(), scene.getScreen(), outputPath);
}

/**
 * @brief Initializes the raytracer_py Python extension module.
 */
PYBIND11_MODULE(raytracer_py, module) {
    module.doc() = "Python bindings for the OOP raytracer project";

    module.def(
        "renderScene",
        &renderScene,
        py::arg("scenePath"),
        py::arg("outputPath")
    );
}
