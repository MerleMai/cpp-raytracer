#include "scene/scene.hpp"

#include <utility>

Scene::Scene(
    const Screen& screen,
    const Medium& medium,
    const std::vector<LightSource>& sources,
    std::vector<std::unique_ptr<Object>> objects
)
    : screen_(screen),
      medium_(medium),
      sources_(sources),
      objects_(std::move(objects)) {}

const Screen& Scene::getScreen() const {
    return screen_;
}

const Medium& Scene::getMedium() const {
    return medium_;
}

const std::vector<LightSource>& Scene::getSources() const {
    return sources_;
}

const std::vector<std::unique_ptr<Object>>& Scene::getObjects() const {
    return objects_;
}