# Final Project Report - OOP Raytracer

* **Team:** Merle Mai, Kevin Fister, Lea Fricke, Annika Neubauer 
* **Date:** 25.09.2026 

---

## 1. Executive Summary & Program Usability

This project is a fully functional C++17 Raytracer with Python bindings.

* **Installation & Build:** Detailed, cross-platform build instructions are available in the [README.md](../README.md).
* **Usage & Examples:** Commands for running the C++ executable and the Python showcase script are documented in the [Usage Section](../README.md#run-the-raytracer).
* **Input Scenes:** Example scene files can be found in the [`examples/`](../examples) directory.

---

## 2. Code Documentation & Architecture

* **UML Class Diagram:** The complete architecture, polymorphic hierarchy, and module interactions are illustrated in our [UML Class Diagram](design.md#current-class-diagram).
* **API Documentation:** All core header files are fully documented using Doxygen standard tags (`@brief`, `@param`, `@return`).
* **Generating Docs:** To build the local HTML documentation via Doxygen, run in the terminal:
    
    First: 
    ```bash
    cmake --build build --target docs
    ```
    Second (on mac): 
    ```bash
    open build/docs/html/index.html
    ```
    Second (under Linux):
    ```bash
    xdg-open build/docs/html/index.html
    ```

---

## 3. Development Quality & Features 

### Core Implementation 

* **Geometry & CSG:** Supports Sphere, HalfSpace, affine transformations (Scaling, Translation, Rotation), and CSG combinations (Union, Intersection, Difference).
* **Lighting Model:** Implementation of Phong shading, ambient/diffuse/specular light, and shadow calculation.
* **Scene Parser:** JSON parsing and validation using nlohmann/json as detailed in the Input Format Specification.

### External Libraries

* **Eigen:** Used for 3D/4D vector and matrix math operations.
* **nlohmann/json:** Utilized for scene input parsing.
* **pybind11:** Integrates the C++ rendering core into Python.
* **OpenCV:** Integrated for writing and encoding rendered image buffers to PNG files.

### Automated Testing

Automated unit and integration tests are executed using CTest. Run them with:
```bash 
ctest --test-dir build --output-on-failure
```

### Additional Features

* **Python Wrapping:** Complete Python binding module provided via [Python binding](../src/python/bindings.cpp)
* **Showcase Script:** Batch-rendering script render_showcase.py for automated rendering of all example scenes.

### Visual Showcase and rendered examples 

| Reflection | CSG Operations | Showcase |
| :---: | :---: | :---: |
| ![Reflection](../examples/showcase/01-reflection.png) | ![CSG](../examples/showcase/04-csg.png) | ![Showcase](../examples/showcase/06-showcase.png) |
| *Reflection* | *CSG Operations* | *Showcase* |

---

## 4. Conclusion & Links

* [Build instructions](../README.md)
* [Design Document](design.md)
* [JSON Input Format Document](inputFormat.md)
* [Showcase scenes](../examples/showcase)




