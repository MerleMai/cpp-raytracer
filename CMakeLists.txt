cmake_minimum_required(VERSION 3.16)

project(raytracer LANGUAGES CXX)

# ---------------------------------------------------------------------------
# C++ configuration
# ---------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Keep the project warning-free during development.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
elseif(MSVC)
    add_compile_options(/W4)
endif()

# ---------------------------------------------------------------------------
# External dependencies
# ---------------------------------------------------------------------------

find_package(Eigen3 REQUIRED NO_MODULE)
find_package(nlohmann_json REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------
# Core raytracer library
# ---------------------------------------------------------------------------

add_library(raytracer_core
    src/geometry/material.cpp
    src/geometry/primitiveObject.cpp
    src/geometry/sphere.cpp
    src/geometry/halfSpace.cpp

    src/geometry/ray.cpp

    src/geometry/transformation.cpp
    src/geometry/scaling.cpp
    src/geometry/translation.cpp
    src/geometry/rotation.cpp

    src/geometry/objectCombination.cpp
    src/geometry/union.cpp
    src/geometry/intersection.cpp
    src/geometry/difference.cpp

    src/scene/screen.cpp
    src/scene/medium.cpp
    src/scene/lightSource.cpp
    src/scene/scene.cpp

    src/io/sceneParser.cpp
    src/io/imageWriter.cpp

    src/render/renderer.cpp
)

target_include_directories(
    raytracer_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
    raytracer_core
    PUBLIC
        Eigen3::Eigen
        nlohmann_json::nlohmann_json
        Threads::Threads
    PRIVATE
        ${OpenCV_LIBS}
)

add_executable(
    raytracer
    src/main.cpp
)

target_link_libraries(
    raytracer
    PRIVATE
        raytracer_core
)

# ---------------------------------------------------------------------------
# Optional Python bindings
# ---------------------------------------------------------------------------

# Use the modern FindPython module instead of the deprecated
# FindPythonLibs (avoids CMake's CMP0148 developer warning).
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG QUIET)

if(pybind11_FOUND)
    pybind11_add_module(raytracer_py src/python/bindings.cpp)

    target_link_libraries(
        raytracer_py
        PRIVATE
            raytracer_core
    )
else()
    message(
        STATUS
        "pybind11 not found: Python bindings target will not be available."
    )
endif()

# ---------------------------------------------------------------------------
# Automated tests
# ---------------------------------------------------------------------------

enable_testing()

add_executable(
    testSceneParser
    tests/testSceneParser.cpp
)

target_link_libraries(
    testSceneParser
    PRIVATE
        raytracer_core
)

add_test(
    NAME SceneParserTests
    COMMAND testSceneParser
)


add_executable(
    testGeometry
    tests/testGeometry.cpp
)

target_link_libraries(
    testGeometry
    PRIVATE
        raytracer_core
)

add_test(
    NAME GeometryTests
    COMMAND testGeometry
)

# Tests currently load example files using paths relative to the
# project root, so CTest must execute them from this directory.
set_tests_properties(
    SceneParserTests
    PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# ---------------------------------------------------------------------------
# API documentation
# ---------------------------------------------------------------------------

find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "OOP Raytracer")
    set(DOXYGEN_PROJECT_BRIEF "C++ raytracer for the OOP programming project")

    set(
        DOXYGEN_OUTPUT_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/docs
    )

    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)

    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_EXTRACT_ALL NO)
    set(DOXYGEN_EXTRACT_PRIVATE NO)
    set(DOXYGEN_EXTRACT_STATIC YES)

    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_FULL_SIDEBAR YES)

    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_WARN_IF_DOC_ERROR YES)

    if(DOXYGEN_DOT_FOUND)
        set(DOXYGEN_HAVE_DOT YES)
        set(DOXYGEN_CLASS_DIAGRAMS YES)
        set(DOXYGEN_CLASS_GRAPH YES)
        set(DOXYGEN_COLLABORATION_GRAPH YES)
    endif()

    # Use the README as the start page of the generated HTML documentation.
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

    doxygen_add_docs(
        docs
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/eval.md
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/design.md
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/inputFormat.md
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/showcase/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Generate HTML API documentation"
    )
else()
    message(
        STATUS
        "Doxygen not found: API documentation target will not be available."
    )
endif()
