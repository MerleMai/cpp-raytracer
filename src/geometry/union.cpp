#include "geometry/union.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

Union::Union(
    std::vector<std::unique_ptr<Object>> children
)
    : ObjectCombination(std::move(children)) {}

bool Union::contains(const Eigen::Vector4d& x) const {
    const std::vector<std::unique_ptr<Object>>& children = getChildren();

    return std::any_of(
        children.begin(),
        children.end(),
        [&x](const std::unique_ptr<Object>& child) {
            return child->contains(x);
        }
    );
}

std::vector<Hit> Union::intersect(const Ray& ray) const {
    const std::vector<std::unique_ptr<Object>>& children = getChildren();
    std::vector<Hit> hits;

    for (std::size_t owner = 0; owner < children.size(); owner++) {
        for (const Hit& hit : children[owner]->intersect(ray)) {
            bool insideOthers = false;
            for (std::size_t other = 0; other < children.size(); other++) {
                if (other == owner) {
                    continue;
                }
                if (children[other]->contains(hit.point)) {
                    insideOthers = true;
                    break;
                }
            }

            if (!insideOthers) {
                hits.push_back(hit);
            }
        }
    }

    std::sort(
        hits.begin(),
        hits.end(),
        [](const Hit& first, const Hit& second) { return first.t < second.t; }
    );

    return hits;
}
