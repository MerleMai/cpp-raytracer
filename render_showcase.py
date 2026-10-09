"""Batch-render the showcase scenes through the raytracer_py Python module.

Usage:
    python3 render_showcase.py                 # render every scene in examples/showcase
    python3 render_showcase.py 01-reflection   # render selected scenes by name or path

Images are written to out/<scene-name>.png. The module is imported from the
CMake build directory, so the project must be built with pybind11 available
(see README.md). Exits with status 1 if any scene fails to render.
"""

import pathlib
import sys
import time

projectRoot = pathlib.Path(__file__).resolve().parent
buildDirectory = projectRoot / "build"
sceneDirectory = projectRoot / "examples" / "showcase"
outputDirectory = projectRoot / "out"

sys.path.insert(0, str(buildDirectory))

try:
    import raytracer_py
except ImportError:
    print(f"raytracer_py not found in {buildDirectory}")
    print("build it first: cmake --build build --target raytracer_py")
    raise SystemExit(1)


def main():
    """Render the requested scenes, print per-scene timing, and exit non-zero on failure."""
    arguments = sys.argv[1:]
    if arguments:
        scenePaths = [sceneDirectory / f"{name}.json" if not name.endswith(".json") else pathlib.Path(name) for name in arguments]
    else:
        scenePaths = sorted(sceneDirectory.glob("*.json"))

    if not scenePaths:
        print(f"no scenes found in {sceneDirectory}")
        raise SystemExit(1)

    outputDirectory.mkdir(exist_ok=True)

    failures = 0
    totalStart = time.perf_counter()

    for scenePath in scenePaths:
        if not scenePath.is_file():
            print(f"{scenePath.name}: not found")
            failures += 1
            continue

        outputPath = outputDirectory / f"{scenePath.stem}.png"
        start = time.perf_counter()
        try:
            raytracer_py.renderScene(str(scenePath), str(outputPath))
        except Exception as error:
            print(f"{scenePath.name}: failed - {error}")
            failures += 1
            continue
        seconds = time.perf_counter() - start
        print(f"{scenePath.name} -> {outputPath.relative_to(projectRoot)} ({seconds:.1f} s)")

    totalSeconds = time.perf_counter() - totalStart
    print(f"{len(scenePaths) - failures}/{len(scenePaths)} scenes rendered in {totalSeconds:.1f} s")

    if failures:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
