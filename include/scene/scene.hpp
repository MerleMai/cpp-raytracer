#pragma once

#include <vector>
#include <memory>
#include "scene/screen.hpp"
#include "scene/medium.hpp"
#include "scene/lightSource.hpp"
#include "geometry/object.hpp"

/**
 * @brief Stores all data required to describe a complete raytracing scene.
 * 
 * A scene contains the screen and observer configuration, the optical
 * properties of the surrounding medium, all light sources, and the complete
 * geometric object tree. The scene exclusively owns every root object; each
 * root may recursively own transformations, combinations, and primitives.
 */
class Scene {
private:
    /// @brief Screen and observer configuration of the scene
    Screen screen_;

    /// @brief Global optical properties of the surrounding medium
    Medium medium_;

    /// @brief Collection of light sources in the scene
    std::vector<LightSource> sources_;

    /// @brief Collection of geometric root objects contained in the scene.
    std::vector<std::unique_ptr<Object>> objects_;
public:
    /**
    * @brief Constructs a scene from its screen, medium, light sources, and objects.
    * 
    * Takes ownership of the geometric objects by moving the unique_ptr vector into the internal storage. 
    *
    * @param screen Screen and observer configuration.
    * @param medium Global optical properties of the surrounding medium.
    * @param sources Collection of light sources contained in the scene.
    * @param objects Collection of owned geometric root objects. Ownership is transferred to the scene.
    */
    Scene(
        const Screen& screen,
        const Medium& medium,
        const std::vector<LightSource>& sources,
        std::vector<std::unique_ptr<Object>> objects
    );

    /**
    * @brief Provides access to the collection of geometric objects in the scene.
    *
    * @return Constant reference to the vector of unique pointers to owned root objects.
    */
    const std::vector<std::unique_ptr<Object>>& getObjects() const;

    /**
     * @brief Provides access to the screen and observer configuration.
     * 
     * @return Constant reference to the scene's screen.
     */
    const Screen& getScreen() const;

    /**
     * @brief Provides access to the global optical properties of the scene.
     * 
     * @return Constant reference to the surrounding medium.
     */
    const Medium& getMedium() const;

    /**
     * @brief Provides access to all light sources contained in the scene.
     * 
     * @return Constant reference to the collection of light sources.
     */
    const std::vector<LightSource>& getSources() const;
};
