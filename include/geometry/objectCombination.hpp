#pragma once

#include <memory>
#include <vector>

#include "geometry/object.hpp"

/**
 * @brief Base class for logical combinations of scene objects.
 *
 * A combination exclusively owns an ordered collection of child objects.
 * Each child may be a primitive, transformation, or another combination.
 */
class ObjectCombination : public Object {
private:
    /// @brief Child objects participating in the logical operation.
    std::vector<std::unique_ptr<Object>> children_;

protected:
    /**
     * @brief Constructs an object combination from its child objects.
     *
     * @param children Objects owned by the combination.
     */
    explicit ObjectCombination(
        std::vector<std::unique_ptr<Object>> children
    );

public:
    /**
     * @brief Provides access to the collection of child objects.
     *
     * @return Constant reference to the owned child-object collection.
     */
    const std::vector<std::unique_ptr<Object>>& getChildren() const;
};
