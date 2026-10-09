#include "geometry/difference.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

Difference::Difference(
    std::vector<std::unique_ptr<Object>> children
)
    : ObjectCombination(std::move(children)) {}

bool Difference::contains(const Eigen::Vector4d& x) const {
    const std::vector<std::unique_ptr<Object>>& children = getChildren();

    if (children.empty()) {
        return false;
    }

    if (!children.front()->contains(x)) {
        return false;
    }

    for (std::size_t i = 1; i < children.size(); i++) {
        if (children[i]->contains(x)) {
            return false;
        }
    }

    return true;
}

std::vector<Hit> Difference::intersect(const Ray& ray) const {
    const std::vector<std::unique_ptr<Object>>& children = getChildren();
    std::vector<Hit> hits;

    if (children.empty()) {
        return hits;
    }

    const Object& base = *children.front();

    for (const Hit& hit : base.intersect(ray)) {
        bool insideSubtracted = false;
        for (std::size_t i = 1; i < children.size(); i++) {
            if (children[i]->contains(hit.point)) {
                insideSubtracted = true;
                break;
            }
        }

        if (!insideSubtracted) {
            hits.push_back(hit);
        }
    }

    for (std::size_t owner = 1; owner < children.size(); owner++) {
        for (Hit hit : children[owner]->intersect(ray)) {
            if (!base.contains(hit.point)) {
                continue;
            }

            bool insideOtherSubtracted = false;
            for (std::size_t other = 1; other < children.size(); other++) {
                if (other != owner && children[other]->contains(hit.point)) {
                    insideOtherSubtracted = true;
                    break;
                }
            }
            if (insideOtherSubtracted) {
                continue;
            }

            hit.normal = -hit.normal;
            hit.entering = !hit.entering;
            hits.push_back(hit);
        }
    }

    std::sort(
        hits.begin(),
        hits.end(),
        [](const Hit& first, const Hit& second) { return first.t < second.t; }
    );

    return hits;
}
