#pragma once

#include <string>
#include "scene/scene.hpp"

/**
 * @brief Parses JSON scene description files into internal Scene data models.
 * 
 * Scene Parser converts the external JSON scene representation into
 * the internal Scene data model used by the rest of the program.
 * It validates documented constraints and normalizes half-space normals.
 * Rendering code therefore does not need to depend on the JSON format.
 */

class SceneParser {
public:
    /**
     * @brief Loads, parses, and validates a complete scene from a JSON file.
     * 
     * Reads the specified JSON file and recursively constructs the screen, medium,
     * light sources, and object hierarchy.
     * 
     * @param filename Path to the JSON scene description.
     * 
     * @return Parsed and validated Scene object.
     * 
     * @throws std::runtime_error If the file cannot be opened or if the scene description is invalid.
     */
    static Scene load(const std::string& filename);
};
