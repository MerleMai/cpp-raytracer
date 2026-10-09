#include "geometry/objectCombination.hpp"

#include <utility>

ObjectCombination::ObjectCombination(
    std::vector<std::unique_ptr<Object>> children
)
    : children_(std::move(children)) {}

const std::vector<std::unique_ptr<Object>>&
ObjectCombination::getChildren() const {
    return children_;
}