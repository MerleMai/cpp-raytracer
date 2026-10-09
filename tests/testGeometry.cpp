#include <cmath>
#include <stdexcept>
#include <string>
#include <exception>
#include <iostream>
#include <Eigen/Core>
#include <memory>

#include "geometry/translation.hpp"
#include "geometry/ray.hpp"
#include "geometry/difference.hpp"
#include "geometry/intersection.hpp"
#include "geometry/rotation.hpp"
#include "geometry/union.hpp"
#include "geometry/material.hpp"
#include "geometry/sphere.hpp"
#include "geometry/halfSpace.hpp"
#include "geometry/scaling.hpp"
#include "render/color.hpp"

#include "testSupport.hpp"

/**
 * @brief Tests Ray::at().
 *
 * Case chosen so that a forgotten summand or an ignored t immediately
 * shows up: origin has no zero components, and t != 1.
 *
 *   origin    = (1, 0, 0, 1)
 *   direction = (0, 1, 0, 0)
 *   t         = 2
 *   expected  = (1, 2, 0, 1)
 */
void testRayAt()
{
    const Ray ray(
        Eigen::Vector4d(1.0, 0.0, 0.0, 1.0),
        Eigen::Vector4d(0.0, 1.0, 0.0, 0.0));

    const Eigen::Vector4d point = ray.at(2.0);

    EXPECT(nearlyEqual(point.x(), 1.0));
    EXPECT(nearlyEqual(point.y(), 2.0));
    EXPECT(nearlyEqual(point.z(), 0.0));
    EXPECT(nearlyEqual(point.w(), 1.0));
}
/**
 * @brief Tests Sphere::intersect() against a hand-computed case.
 *
 * Unit sphere at the origin, ray coming straight along +z from behind it:
 *
 *   sphere    = center (0,0,0), radius 1
 *   origin    = (0, 0, -5, 1)
 *   direction = (0, 0, 1, 0)   (already unit length)
 *
 * The ray enters the sphere at t=4 (point (0,0,-1)) and exits at t=6
 * (point (0,0,1)).
 */
void testSphereIntersect()
{
    const Sphere sphere(
        Eigen::Vector4d(0.0, 0.0, 0.0, 1.0),
        1.0,
        Material(),
        1.0);

    const Ray ray(
        Eigen::Vector4d(0.0, 0.0, -5.0, 1.0),
        Eigen::Vector4d(0.0, 0.0, 1.0, 0.0));

    const std::vector<Hit> hits = sphere.intersect(ray);

    EXPECT(hits.size() == 2);

    EXPECT(nearlyEqual(hits[0].t, 4.0));
    EXPECT(nearlyEqual(hits[0].point.z(), -1.0));
    EXPECT(nearlyEqual(hits[0].normal.z(), -1.0));
    EXPECT(hits[0].entering);

    EXPECT(nearlyEqual(hits[1].t, 6.0));
    EXPECT(nearlyEqual(hits[1].point.z(), 1.0));
    EXPECT(nearlyEqual(hits[1].normal.z(), 1.0));
    EXPECT(!hits[1].entering);
}

/**
 * @brief Tests HalfSpace::intersect() against a hand-computed case.
 *
 * Half-space through the origin with normal (0,0,1) (i.e. z >= 0 is
 * outside), ray coming from below along +z:
 *
 *   position  = (0, 0, 0, 1)
 *   normal    = (0, 0, 1, 0)
 *   origin    = (0, 0, -2, 1)
 *   direction = (0, 0, 1, 0)
 *
 * Expected: t=2, intersection point (0, 0, 0).
 */
void testHalfSpaceIntersect()
{
    const HalfSpace plane(
        Eigen::Vector4d(0.0, 0.0, 0.0, 1.0),
        Eigen::Vector4d(0.0, 0.0, 1.0, 0.0),
        Material(),
        1.0);

    const Ray ray(
        Eigen::Vector4d(0.0, 0.0, -2.0, 1.0),
        Eigen::Vector4d(0.0, 0.0, 1.0, 0.0));

    const std::vector<Hit> hits = plane.intersect(ray);

    EXPECT(hits.size() == 1);
    EXPECT(nearlyEqual(hits[0].t, 2.0));
    EXPECT(nearlyEqual(hits[0].point.x(), 0.0));
    EXPECT(nearlyEqual(hits[0].point.y(), 0.0));
    EXPECT(nearlyEqual(hits[0].point.z(), 0.0));
}

/**
 * @brief Tests Translation::intersect().
 *
 * Same unit sphere and ray shape as testSphereIntersect(), but the sphere
 * is translated by (5, 0, 0) and the ray is shifted to match. The hit
 * parameters t must stay the same as the untranslated case, while the
 * hit points must be shifted by the translation offset.
 */
void testTranslationIntersect()
{
    auto sphere = std::make_unique<Sphere>(
        Eigen::Vector4d(0.0, 0.0, 0.0, 1.0),
        1.0,
        Material(),
        1.0);

    const Translation translated(
        Eigen::Vector3d(5.0, 0.0, 0.0),
        std::move(sphere));

    const Ray ray(
        Eigen::Vector4d(5.0, 0.0, -5.0, 1.0),
        Eigen::Vector4d(0.0, 0.0, 1.0, 0.0));

    const std::vector<Hit> hits = translated.intersect(ray);

    EXPECT(hits.size() == 2);
    EXPECT(nearlyEqual(hits[0].t, 4.0));
    EXPECT(nearlyEqual(hits[0].point.x(), 5.0));
    EXPECT(nearlyEqual(hits[0].point.z(), -1.0));

    EXPECT(nearlyEqual(hits[1].t, 6.0));
    EXPECT(nearlyEqual(hits[1].point.x(), 5.0));
    EXPECT(nearlyEqual(hits[1].point.z(), 1.0));
}
/**
 * @brief Tests that Scaling::intersect() corrects the normal correctly.
 *
 * A unit sphere scaled uniformly by factor 2 behaves like a radius-2
 * sphere for intersection distances (t=3 and t=7 instead of t=4 and
 * t=6). Because the scaling is uniform, the surface normal must still
 * be unit length after the correction.
 */
void testScalingNormal()
{
    auto sphere = std::make_unique<Sphere>(
        Eigen::Vector4d(0.0, 0.0, 0.0, 1.0),
        1.0,
        Material(),
        1.0);

    const Scaling scaled(
        Eigen::Vector3d(2.0, 2.0, 2.0),
        std::move(sphere));

    const Ray ray(
        Eigen::Vector4d(0.0, 0.0, -5.0, 1.0),
        Eigen::Vector4d(0.0, 0.0, 1.0, 0.0));

    const std::vector<Hit> hits = scaled.intersect(ray);

    EXPECT(hits.size() == 2);

    EXPECT(nearlyEqual(hits[0].t, 3.0));
    EXPECT(nearlyEqual(hits[0].point.z(), -2.0));
    EXPECT(nearlyEqual(hits[0].normal.z(), -1.0));
    EXPECT(nearlyEqual(hits[0].normal.head<3>().norm(), 1.0));

    EXPECT(nearlyEqual(hits[1].t, 7.0));
    EXPECT(nearlyEqual(hits[1].point.z(), 2.0));
    EXPECT(nearlyEqual(hits[1].normal.z(), 1.0));
    EXPECT(nearlyEqual(hits[1].normal.head<3>().norm(), 1.0));
}

/**
 * @brief Tests the light-combination operator from render/color.hpp.
 *
 * Checks the formula directly against a hand-computed value, and checks
 * associativity: combine(combine(a, b), c) must equal combine(a,
 * combine(b, c)).
 */
void testCombine()
{
    const Eigen::Vector3d half(0.5,0.5, 0.5);
    const Eigen::Vector3d combinedHalves = combine(half,half);

    EXPECT(nearlyEqual(combinedHalves.x(), 0.75));
    EXPECT(nearlyEqual(combinedHalves.y(), 0.75));
    EXPECT(nearlyEqual(combinedHalves.z(), 0.75));

    const Eigen::Vector3d a(0.2, 0.4, 0.6);
    const Eigen::Vector3d b(0.3, 0.1, 0.9);
    const Eigen::Vector3d c(0.1, 0.1, 0.1);

    const Eigen::Vector3d left = combine(combine(a, b), c);
    const Eigen::Vector3d right = combine(a, combine(b, c));

    EXPECT(nearlyEqual(left.x(), right.x()));
    EXPECT(nearlyEqual(left.y(), right.y()));
    EXPECT(nearlyEqual(left.z(), right.z()));

    EXPECT(nearlyEqual(left.x(), 0.496));
    EXPECT(nearlyEqual(left.y(), 0.514));
    EXPECT(nearlyEqual(left.z(), 0.964));
}
/**
 * @brief Tests Rotation::intersect().
 *
 * A HalfSpace with normal (1,0,0) (the plane x=0) is rotated 90 degrees
 * around Z, which turns it into the plane y=0 with its normal now
 * pointing along +y. A ray starting at (0,-5,0) and flying in +y
 * direction must hit that rotated plane at y=0, i.e. at t=5, at the
 * point (0,0,0), with the surface normal now reported as (0,1,0).
 */
void testRotationIntersect()
{
    auto plane = std::make_unique<HalfSpace>(
        Eigen::Vector4d(0.0, 0.0, 0.0, 1.0),
        Eigen::Vector4d(1.0, 0.0, 0.0, 0.0),
        Material(),
        1.0
    );

    const Rotation rotated(90.0, RotationAxis::Z, std::move(plane));

    const Ray ray(
        Eigen::Vector4d(0.0, -5.0, 0.0, 1.0),
        Eigen::Vector4d(0.0, 1.0, 0.0, 0.0));

    const std::vector<Hit> hits = rotated.intersect(ray);

    EXPECT(hits.size() == 1);
    EXPECT(nearlyEqual(hits[0].t, 5.0));
    EXPECT(nearlyEqual(hits[0].point.x(), 0.0));
    EXPECT(nearlyEqual(hits[0].point.y(), 0.0));
    EXPECT(nearlyEqual(hits[0].point.z(), 0.0));
    EXPECT(nearlyEqual(hits[0].normal.x(), 0.0));
    EXPECT(nearlyEqual(hits[0].normal.y(), 1.0));
    EXPECT(nearlyEqual(hits[0].normal.z(), 0.0));
    EXPECT(!hits[0].entering);
}

/**
 * @brief Tests Union::intersect() with two overlapping spheres.
 *
 * Sphere A: center (0,0,0), radius 2. Sphere B: center (0,0,1), radius 2.
 * A ray fired along +z from far behind both must only report the two
 * outer surface crossings of the combined blob: t=8 (entering through
 * sphere A) and t=13 (leaving through sphere B). The two "inner"
 * crossings inside the overlap must be discarded.
 */
void testUnionIntersect()
{
    std::vector<std::unique_ptr<Object>> spheres;
    spheres.push_back(std::make_unique<Sphere>(
        Eigen::Vector4d(0.0, 0.0, 0.0, 1.0), 2.0, Material(), 1.0)
    );
    spheres.push_back(std::make_unique<Sphere>(
        Eigen::Vector4d(0.0, 0.0, 1.0, 1.0), 2.0, Material(), 1.0)
    );

    const Union both(std::move(spheres));

    const Ray ray(
        Eigen::Vector4d(0.0, 0.0, -10.0, 1.0),
        Eigen::Vector4d(0.0, 0.0, 1.0, 0.0)
    );
    const std::vector<Hit> hits = both.intersect(ray);

    EXPECT(hits.size() == 2);
    EXPECT(nearlyEqual(hits[0].t, 8.0));
    EXPECT(nearlyEqual(hits[0].point.z(), -2.0));

    EXPECT(nearlyEqual(hits[1].t, 13.0));
    EXPECT(nearlyEqual(hits[1].point.z(), 3.0));
}

/**
 * @brief Tests Intersection::intersect() with the same two spheres.
 *
 * Only the shared "lens" region counts. The ray enters that shared
 * region at t=9 (entering sphere B, already inside sphere A) and
 * leaves it at t=12 (exiting sphere A, still inside sphere B). The two
 * crossings outside the overlap (t=8, t=13) must be discarded here.
 */
void testIntersectionIntersect()
{
    std::vector<std::unique_ptr<Object>> spheres;
    spheres.push_back(std::make_unique<Sphere>(
        Eigen::Vector4d(0.0, 0.0, 0.0, 1.0), 2.0, Material(), 1.0));
    spheres.push_back(std::make_unique<Sphere>(
        Eigen::Vector4d(0.0, 0.0, 1.0, 1.0), 2.0, Material(), 1.0));

    const Intersection overlap(std::move(spheres));

    const Ray ray(
        Eigen::Vector4d(0.0, 0.0, -10.0, 1.0),
        Eigen::Vector4d(0.0, 0.0, 1.0, 0.0)
    );

    const std::vector<Hit> hits = overlap.intersect(ray);

    EXPECT(hits.size() == 2);

    EXPECT(nearlyEqual(hits[0].t, 9.0));
    EXPECT(nearlyEqual(hits[0].point.z(), -1.0));

    EXPECT(nearlyEqual(hits[1].t, 12.0));
    EXPECT(nearlyEqual(hits[1].point.z(), 2.0));
}

/**
 * @brief Tests Difference::intersect() with a sphere minus an off-center sphere.
 *
 * A big sphere (center (0,0,0), radius 3) has a small sphere (center
 * (1,0,0), radius 1) punched out of it, like an apple with a wormhole
 * bored through one side. A ray fired along +x from far outside must
 * report four hits: t=7 (entering the apple's own surface, untouched),
 * t=10 and t=12 (the inside walls of the wormhole, with normal and
 * entering flipped, since this surface now belongs to the hole seen
 * from inside), and t=13 (leaving the apple's far surface, untouched).
 */
void testDifferenceIntersect()
{
    std::vector<std::unique_ptr<Object>> children;
    children.push_back(std::make_unique<Sphere>(
        Eigen::Vector4d(0.0, 0.0, 0.0, 1.0), 3.0, Material(), 1.0)
    );

    auto hole = std::make_unique<Sphere>(
        Eigen::Vector4d(0.0, 0.0, 0.0, 1.0), 1.0, Material(), 1.0
    );
    children.push_back(std::make_unique<Translation>(
        Eigen::Vector3d(1.0, 0.0, 0.0), std::move(hole)
    ));

    const Difference apple(std::move(children));

    const Ray ray(
        Eigen::Vector4d(-10.0, 0.0, 0.0, 1.0),
        Eigen::Vector4d(1.0, 0.0, 0.0, 0.0));

    const std::vector<Hit> hits = apple.intersect(ray);

    EXPECT(hits.size() == 4 );

    EXPECT(nearlyEqual(hits[0].t, 7.0));
    EXPECT(nearlyEqual(hits[0].point.x(), -3.0));
    EXPECT(nearlyEqual(hits[0].normal.x(), -1.0));
    EXPECT(hits[0].entering);

    EXPECT(nearlyEqual(hits[1].t, 10.0));
    EXPECT(nearlyEqual(hits[1].point.x(), 0.0));
    EXPECT(nearlyEqual(hits[1].normal.x(), 1.0));
    EXPECT(!hits[1].entering);

    EXPECT(nearlyEqual(hits[2].t, 12.0));
    EXPECT(nearlyEqual(hits[2].point.x(), 2.0));
    EXPECT(nearlyEqual(hits[2].normal.x(), -1.0));
    EXPECT(hits[2].entering);

    EXPECT(nearlyEqual(hits[3].t, 13.0));
    EXPECT(nearlyEqual(hits[3].point.x(), 3.0));
    EXPECT(nearlyEqual(hits[3].normal.x(), 1.0));
    EXPECT(!hits[3].entering);
}

/**
 * @brief Runs all geometry tests.
 * @return Zero if all geometry tests pass, otherwise one.
 */
int main() {
    try {
        testRayAt();
        testSphereIntersect();
        testHalfSpaceIntersect();
        testTranslationIntersect();
        testScalingNormal();
        testRotationIntersect();
        testUnionIntersect();
        testIntersectionIntersect();
        testDifferenceIntersect();
        testCombine();

        std::cout
            << "Geometry test passed. \n";

        return 0;
    }
    catch (const std::exception& error) {
        std::cerr
            << "Geometry test failed: "
            << error.what()
            << '\n';

        return 1;
    }
}