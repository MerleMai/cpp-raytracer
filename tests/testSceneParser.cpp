#include <cmath>
#include <cstdio>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

#include "geometry/halfSpace.hpp"
#include "geometry/material.hpp"
#include "geometry/rotation.hpp"
#include "geometry/scaling.hpp"
#include "geometry/sphere.hpp"
#include "geometry/translation.hpp"
#include "geometry/difference.hpp"
#include "geometry/intersection.hpp"
#include "geometry/union.hpp"
#include "io/sceneParser.hpp"

#include "testSupport.hpp"

/**
 * @brief Writes temporary JSON content used by parser validation tests.
 *
 * @param filename Path of the temporary file.
 * @param content JSON content written to the file.
 *
 * @throws std::runtime_error If the test file cannot be created.
 */
void writeTestFile(
    const std::string& filename,
    const std::string& content
) {
    std::ofstream output(filename);

    if (!output.is_open()) {
        throw std::runtime_error(
            "Could not create temporary test file: " + filename
        );
    }

    output << content;
}

/**
 * @brief Verifies that an operation throws std::runtime_error.
 *
 * @param operation Operation expected to fail.
 */
void expectRuntimeError(
    const std::function<void()>& operation
) {
    bool thrown = false;

    try {
        operation();
    }
    catch (const std::runtime_error&) {
        thrown = true;
    }

    EXPECT(thrown);
}

/**
 * @brief Tests screen, medium, and light-source parsing.
 */
void testBasicScene() {
    const Scene scene =
        SceneParser::load("examples/basicScene.json");

    // Screen
    EXPECT(scene.getScreen().getDpi() == 128);

    const auto& dimensions =
        scene.getScreen().getDimensions();

    EXPECT(nearlyEqual(dimensions.x(), 2.0));
    EXPECT(nearlyEqual(dimensions.y(), 4.0));

    const auto& screenPosition =
        scene.getScreen().getPosition();

    EXPECT(nearlyEqual(screenPosition.x(), -1.0));
    EXPECT(nearlyEqual(screenPosition.y(), -2.0));
    EXPECT(nearlyEqual(screenPosition.z(), -10.0));
    EXPECT(nearlyEqual(screenPosition.w(), 1.0));

    const auto& observer =
        scene.getScreen().getObserver();

    EXPECT(nearlyEqual(observer.x(), 0.0));
    EXPECT(nearlyEqual(observer.y(), 0.0));
    EXPECT(nearlyEqual(observer.z(), -20.0));
    EXPECT(nearlyEqual(observer.w(), 1.0));

    // Medium
    const auto& ambient =
        scene.getMedium().getAmbient();

    EXPECT(nearlyEqual(ambient.x(), 0.1));
    EXPECT(nearlyEqual(ambient.y(), 0.1));
    EXPECT(nearlyEqual(ambient.z(), 0.1));

    EXPECT(nearlyEqual(
        scene.getMedium().getIndex(),
        1.0
    ));

    EXPECT(scene.getMedium().getRecursion() == 1);

    // Light sources
    const auto& sources =
        scene.getSources();

    EXPECT(sources.size() == 2);

    EXPECT(nearlyEqual(
        sources[0].getPosition().x(),
        0.0
    ));

    EXPECT(nearlyEqual(
        sources[0].getIntensity().x(),
        1.0
    ));

    EXPECT(nearlyEqual(
        sources[1].getPosition().x(),
        1.0
    ));

    EXPECT(nearlyEqual(
        sources[1].getIntensity().x(),
        0.5
    ));
}

/**
 * @brief Tests primitive-object parsing and material defaults.
 */
void testPrimitiveScene() {
    const Scene scene =
        SceneParser::load("examples/primitiveScene.json");

    EXPECT(scene.getObjects().size() == 2);

    // Sphere
    const auto* sphere =
        dynamic_cast<const Sphere*>(
            scene.getObjects()[0].get()
        );

    EXPECT(sphere != nullptr);

    EXPECT(nearlyEqual(
        sphere->getRadius(),
        1.0
    ));

    EXPECT(nearlyEqual(
        sphere->getPosition().w(),
        1.0
    ));

    EXPECT(nearlyEqual(
        sphere->getIndex(),
        1.5
    ));

    const Material& sphereMaterial =
        sphere->getMaterial();

    // Explicitly parsed coefficients
    EXPECT(nearlyEqual(
        sphereMaterial.getAmbient().x(),
        0.1
    ));

    EXPECT(nearlyEqual(
        sphereMaterial.getDiffuse().x(),
        1.0
    ));

    EXPECT(nearlyEqual(
        sphereMaterial.getShininess(),
        10.0
    ));

    // Missing coefficients must use defaults.
    EXPECT(nearlyEqual(
        sphereMaterial.getReflected().x(),
        0.0
    ));

    EXPECT(nearlyEqual(
        sphereMaterial.getReflected().y(),
        0.0
    ));

    EXPECT(nearlyEqual(
        sphereMaterial.getReflected().z(),
        0.0
    ));

    EXPECT(nearlyEqual(
        sphereMaterial.getRefracted().x(),
        0.0
    ));

    EXPECT(nearlyEqual(
        sphereMaterial.getRefracted().y(),
        0.0
    ));

    EXPECT(nearlyEqual(
        sphereMaterial.getRefracted().z(),
        0.0
    ));

    // Half-space; the input normal [0, 2, 0] must be normalized.
    const auto* halfSpace =
        dynamic_cast<const HalfSpace*>(
            scene.getObjects()[1].get()
        );

    EXPECT(halfSpace != nullptr);

    EXPECT(nearlyEqual(
        halfSpace->getPosition().w(),
        1.0
    ));

    EXPECT(nearlyEqual(
        halfSpace->getNormal().x(),
        0.0
    ));

    EXPECT(nearlyEqual(
        halfSpace->getNormal().y(),
        1.0
    ));

    EXPECT(nearlyEqual(
        halfSpace->getNormal().z(),
        0.0
    ));

    EXPECT(nearlyEqual(
        halfSpace->getNormal().w(),
        0.0
    ));
}

/**
 * @brief Tests recursive transformation parsing.
 *
 * The test scene contains:
 *
 * Scaling
 *   -> Translation
 *      -> Rotation
 *         -> Sphere
 */
void testTransformationScene() {
    const Scene scene =
        SceneParser::load(
            "examples/transformationScene.json"
        );

    EXPECT(scene.getObjects().size() == 1);

    // Scaling
    const auto* scaling =
        dynamic_cast<const Scaling*>(
            scene.getObjects()[0].get()
        );

    EXPECT(scaling != nullptr);

    EXPECT(nearlyEqual(
        scaling->getFactors().x(),
        2.0
    ));

    EXPECT(nearlyEqual(
        scaling->getFactors().y(),
        3.0
    ));

    EXPECT(nearlyEqual(
        scaling->getFactors().z(),
        4.0
    ));

    // Translation nested inside scaling
    const auto* translation =
        dynamic_cast<const Translation*>(
            &scaling->getSubject()
        );

    EXPECT(translation != nullptr);

    EXPECT(nearlyEqual(
        translation->getFactors().x(),
        1.0
    ));

    EXPECT(nearlyEqual(
        translation->getFactors().y(),
        2.0
    ));

    EXPECT(nearlyEqual(
        translation->getFactors().z(),
        3.0
    ));

    // Rotation nested inside translation
    const auto* rotation =
        dynamic_cast<const Rotation*>(
            &translation->getSubject()
        );

    EXPECT(rotation != nullptr);

    EXPECT(nearlyEqual(
        rotation->getAngleDegrees(),
        90.0
    ));

    EXPECT(
        rotation->getAxis() ==
        RotationAxis::Z
    );

    // Sphere nested inside rotation
    const auto* sphere =
        dynamic_cast<const Sphere*>(
            &rotation->getSubject()
        );

    EXPECT(sphere != nullptr);

    EXPECT(nearlyEqual(
        sphere->getRadius(),
        1.0
    ));
}

/**
 * @brief Tests recursive logical object-combination parsing.
 *
 * The scene contains a union with nested transformations,
 * intersection, difference, and primitive objects.
 */
void testCombinationScene() {
    const Scene scene =
        SceneParser::load(
            "examples/combinationScene.json"
        );

    EXPECT(scene.getObjects().size() == 1);

    const auto* unionObject =
        dynamic_cast<const Union*>(
            scene.getObjects()[0].get()
        );

    EXPECT(unionObject != nullptr);
    EXPECT(unionObject->getChildren().size() == 2);

    // First union child: Scaling -> Sphere
    const auto* scaling =
        dynamic_cast<const Scaling*>(
            unionObject->getChildren()[0].get()
        );

    EXPECT(scaling != nullptr);

    const auto* scaledSphere =
        dynamic_cast<const Sphere*>(
            &scaling->getSubject()
        );

    EXPECT(scaledSphere != nullptr);
    EXPECT(nearlyEqual(
        scaledSphere->getRadius(),
        1.0
    ));

    // Second union child: Intersection
    const auto* intersection =
        dynamic_cast<const Intersection*>(
            unionObject->getChildren()[1].get()
        );

    EXPECT(intersection != nullptr);
    EXPECT(intersection->getChildren().size() == 2);

    const auto* halfSpace =
        dynamic_cast<const HalfSpace*>(
            intersection->getChildren()[0].get()
        );

    EXPECT(halfSpace != nullptr);

    // Nested Difference
    const auto* difference =
        dynamic_cast<const Difference*>(
            intersection->getChildren()[1].get()
        );

    EXPECT(difference != nullptr);
    EXPECT(difference->getChildren().size() == 2);

    const auto* firstSphere =
        dynamic_cast<const Sphere*>(
            difference->getChildren()[0].get()
        );

    EXPECT(firstSphere != nullptr);
    EXPECT(nearlyEqual(
        firstSphere->getRadius(),
        3.0
    ));

    const auto* translation =
        dynamic_cast<const Translation*>(
            difference->getChildren()[1].get()
        );

    EXPECT(translation != nullptr);

    const auto* translatedSphere =
        dynamic_cast<const Sphere*>(
            &translation->getSubject()
        );

    EXPECT(translatedSphere != nullptr);
    EXPECT(nearlyEqual(
        translatedSphere->getRadius(),
        1.0
    ));
}

/**
 * @brief Tests rejection of malformed and semantically invalid input.
 */
void testInvalidInput() {
    constexpr const char* filename =
        "invalidScene.json";

    // Missing file
    expectRuntimeError([] {
        SceneParser::load(
            "thisFileDoesNotExist.json"
        );
    });

    // Malformed JSON
    writeTestFile(
        filename,
        R"({
            "screen":
        })"
    );

    expectRuntimeError([] {
        SceneParser::load("invalidScene.json");
    });

    // Invalid dpi
    writeTestFile(
        filename,
        R"({
            "screen": {
                "dpi": 0,
                "dimensions": [2, 4],
                "position": [-1, -2, -10],
                "observer": [0, 0, -20]
            },
            "medium": {
                "ambient": [0.1, 0.1, 0.1],
                "index": 1.0,
                "recursion": 1
            },
            "sources": [],
            "objects": []
        })"
    );

    expectRuntimeError([] {
        SceneParser::load("invalidScene.json");
    });

    // Invalid recursion depth
    writeTestFile(
        filename,
        R"({
            "screen": {
                "dpi": 128,
                "dimensions": [2, 4],
                "position": [-1, -2, -10],
                "observer": [0, 0, -20]
            },
            "medium": {
                "ambient": [0.1, 0.1, 0.1],
                "index": 1.0,
                "recursion": -1
            },
            "sources": [],
            "objects": []
        })"
    );

    expectRuntimeError([] {
        SceneParser::load("invalidScene.json");
    });

    // Zero scaling factor
    writeTestFile(
        filename,
        R"({
            "screen": {
                "dpi": 128,
                "dimensions": [2, 4],
                "position": [-1, -2, -10],
                "observer": [0, 0, -20]
            },
            "medium": {
                "ambient": [0.1, 0.1, 0.1],
                "index": 1.0,
                "recursion": 1
            },
            "sources": [],
            "objects": [
                {
                    "scaling": {
                        "factors": [1, 0, 1],
                        "subject": {
                            "sphere": {
                                "position": [0, 0, 0],
                                "radius": 1,
                                "color": {},
                                "index": 1
                            }
                        }
                    }
                }
            ]
        })"
    );

    expectRuntimeError([] {
        SceneParser::load("invalidScene.json");
    });

    // Transformation without subject
    writeTestFile(
        filename,
        R"({
            "screen": {
                "dpi": 128,
                "dimensions": [2, 4],
                "position": [-1, -2, -10],
                "observer": [0, 0, -20]
            },
            "medium": {
                "ambient": [0.1, 0.1, 0.1],
                "index": 1.0,
                "recursion": 1
            },
            "sources": [],
            "objects": [
                {
                    "translation": {
                        "factors": [1, 2, 3]
                    }
                }
            ]
        })"
    );

    expectRuntimeError([] {
        SceneParser::load("invalidScene.json");
    });

    writeTestFile(
        filename,
        R"({
            "screen": {
                "dpi": 128,
                "dimensions": [2, 4],
                "position": [-1, -2, -10],
                "observer": [0, 0, -20]
            },
            "medium": {
                "ambient": [1.2, 0.1, 0.1],
                "index": 1.0,
                "recursion": 1
            },
            "sources": [],
            "objects": []
        })"
    );

    expectRuntimeError([] {
        SceneParser::load("invalidScene.json");
    });

    writeTestFile(
        filename,
        R"({
            "screen": {
                "dpi": 128,
                "dimensions": [2, 4],
                "position": [-1, -2, -10],
                "observer": [0, 0, -20]
            },
            "medium": {
                "ambient": [0.1, 0.1, 0.1],
                "index": 1.0,
                "recursion": 1
            },
            "sources": [
                {
                    "position": [0, 0, 0],
                    "intensity": [-0.1, 1, 1]
                }
            ],
            "objects": []
        })"
    );

    expectRuntimeError([] {
        SceneParser::load("invalidScene.json");
    });

    writeTestFile(
        filename,
        R"({
            "screen": {
                "dpi": 128,
                "dimensions": [2, 4],
                "position": [-1, -2, -10],
                "observer": [0, 0, -20]
            },
            "medium": {
                "ambient": [0.1, 0.1, 0.1],
                "index": 1.0,
                "recursion": 1
            },
            "sources": [],
            "objects": [
                {
                    "sphere": {
                        "position": [0, 0, 0],
                        "radius": 1,
                        "color": {
                            "diffuse": [0.5, 1.5, 0.5]
                        },
                        "index": 1
                    }
                }
            ]
        })"
    );

    expectRuntimeError([] {
        SceneParser::load("invalidScene.json");
    });

    writeTestFile(
        filename,
        R"({
            "screen": {
                "dpi": 128,
                "dimensions": [2, 4],
                "position": [-1, -2, -10],
                "observer": [0, 0, -20]
            },
            "medium": {
                "ambient": [0.1, 0.1, 0.1],
                "index": 1.0,
                "recursion": 1
            },
            "sources": [],
            "objects": [
                {
                    "sphere": {
                        "position": [0, 0, 0],
                        "radius": 1,
                        "color": {
                            "shininess": -1
                        },
                        "index": 1
                    }
                }
            ]
        })"
    );

    expectRuntimeError([] {
        SceneParser::load("invalidScene.json");
    });

    std::remove(filename);
}

/**
 * @brief Runs all SceneParser tests.
 *
 * @return Zero if all parser tests pass, otherwise one.
 */
int main() {
    try {
        testBasicScene();
        testPrimitiveScene();
        testTransformationScene();
        testCombinationScene();
        testInvalidInput();

        std::cout
            << "Scene parser test passed.\n";

        return 0;
    }
    catch (const std::exception& error) {
        std::remove("invalidScene.json");

        std::cerr
            << "Scene parser test failed: "
            << error.what()
            << '\n';

        return 1;
    }
}
