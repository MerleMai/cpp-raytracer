#include "io/sceneParser.hpp"

#include <cstddef>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <nlohmann/json.hpp>

#include "geometry/halfSpace.hpp"
#include "geometry/material.hpp"
#include "geometry/object.hpp"
#include "geometry/rotation.hpp"
#include "geometry/scaling.hpp"
#include "geometry/sphere.hpp"
#include "geometry/translation.hpp"
#include "geometry/difference.hpp"
#include "geometry/intersection.hpp"
#include "geometry/union.hpp"
#include "scene/lightSource.hpp"
#include "scene/medium.hpp"
#include "scene/screen.hpp"

namespace {

using json = nlohmann::json;

/*
 * Forward declaration required because transformations recursively
 * parse their subject through parseObject().
 */
std::unique_ptr<Object> parseObject(const json& value);

/**
 * @brief Parses a two-component numeric JSON array.
 *
 * @param value JSON value expected to contain exactly two numbers.
 * @param fieldName Name of the field, used for error messages.
 * @return Two-dimensional Eigen vector containing the parsed values.
 *
 * @throws std::runtime_error If the value is not an array containing
 * exactly two numeric elements.
 */
Eigen::Vector2d parseVector2(
    const json& value,
    const std::string& fieldName
) {
    if (!value.is_array() || value.size() != 2) {
        throw std::runtime_error(
            fieldName +
            " must be an array containing exactly 2 values."
        );
    }

    if (!value[0].is_number() ||
        !value[1].is_number()) {
        throw std::runtime_error(
            fieldName +
            " must contain only numeric values."
        );
    }

    return Eigen::Vector2d(
        value[0].get<double>(),
        value[1].get<double>()
    );
}

/**
 * @brief Parses a three-component numeric JSON array.
 *
 * @param value JSON value expected to contain exactly three numbers.
 * @param fieldName Name of the field, used for error messages.
 * @return Three-dimensional Eigen vector containing the parsed values.
 *
 * @throws std::runtime_error If the value is not an array containing
 * exactly three numeric elements.
 */
Eigen::Vector3d parseVector3(
    const json& value,
    const std::string& fieldName
) {
    if (!value.is_array() || value.size() != 3) {
        throw std::runtime_error(
            fieldName +
            " must be an array containing exactly 3 values."
        );
    }

    for (const auto& component : value) {
        if (!component.is_number()) {
            throw std::runtime_error(
                fieldName +
                " must contain only numeric values."
            );
        }
    }

    return Eigen::Vector3d(
        value[0].get<double>(),
        value[1].get<double>(),
        value[2].get<double>()
    );
}

/**
 * @brief Parses and validates an RGB value.
 *
 * RGB components must be numeric values between zero and one.
 *
 * @param value JSON value containing three RGB components.
 * @param fieldName Name of the field, used for error messages.
 * @return Validated RGB vector.
 *
 * @throws std::runtime_error If the value is malformed or contains
 * components outside the range [0, 1].
 */
Eigen::Vector3d parseRgb(
    const json& value,
    const std::string& fieldName
) {
    const Eigen::Vector3d rgb =
        parseVector3(value, fieldName);

    for (int i = 0; i < 3; ++i) {
        if (rgb[i] < 0.0 || rgb[i] > 1.0) {
            throw std::runtime_error(
                fieldName +
                " components must be between 0 and 1."
            );
        }
    }

    return rgb;
}

/**
 * @brief Parses a three-dimensional point into homogeneous coordinates.
 *
 * The three Cartesian input coordinates are converted to
 * (x, y, z, 1).
 *
 * @param value JSON value containing x, y, and z.
 * @param fieldName Name of the field, used for error messages.
 * @return Homogeneous point (x, y, z, 1).
 *
 * @throws std::runtime_error If the input is not a valid
 * three-component numeric array.
 */
Eigen::Vector4d parsePoint(
    const json& value,
    const std::string& fieldName
) {
    const Eigen::Vector3d coordinates =
        parseVector3(value, fieldName);

    return Eigen::Vector4d(
        coordinates.x(),
        coordinates.y(),
        coordinates.z(),
        1.0
    );
}

/**
 * @brief Parses a three-dimensional direction into homogeneous coordinates.
 *
 * The three Cartesian input components are converted to
 * (x, y, z, 0).
 *
 * @param value JSON value containing x, y, and z components.
 * @param fieldName Name of the field, used for error messages.
 * @return Homogeneous direction vector (x, y, z, 0).
 *
 * @throws std::runtime_error If the input is not a valid
 * three-component numeric array.
 */
Eigen::Vector4d parseDirection(
    const json& value,
    const std::string& fieldName
) {
    const Eigen::Vector3d components =
        parseVector3(value, fieldName);

    return Eigen::Vector4d(
        components.x(),
        components.y(),
        components.z(),
        0.0
    );
}

/**
 * @brief Parses the screen configuration of a scene.
 *
 * @param root Root JSON object containing the screen entry.
 * @return Parsed Screen object.
 *
 * @throws std::runtime_error If required screen parameters are missing
 * or contain invalid values.
 */
Screen parseScreen(const json& root) {
    if (!root.contains("screen") ||
        !root["screen"].is_object()) {
        throw std::runtime_error(
            "Scene must contain a valid 'screen' object."
        );
    }

    const json& screen = root["screen"];

    if (!screen.contains("dpi") ||
        !screen["dpi"].is_number_integer()) {
        throw std::runtime_error(
            "screen.dpi must be an integer."
        );
    }

    if (!screen.contains("dimensions")) {
        throw std::runtime_error(
            "screen.dimensions is required."
        );
    }

    if (!screen.contains("position")) {
        throw std::runtime_error(
            "screen.position is required."
        );
    }

    if (!screen.contains("observer")) {
        throw std::runtime_error(
            "screen.observer is required."
        );
    }

    const int dpi =
        screen["dpi"].get<int>();

    if (dpi <= 0) {
        throw std::runtime_error(
            "screen.dpi must be greater than zero."
        );
    }

    const Eigen::Vector2d dimensions =
        parseVector2(
            screen["dimensions"],
            "screen.dimensions"
        );

    if (dimensions.x() <= 0.0 ||
        dimensions.y() <= 0.0) {
        throw std::runtime_error(
            "screen.dimensions values must be greater than zero."
        );
    }

    const Eigen::Vector4d position =
        parsePoint(
            screen["position"],
            "screen.position"
        );

    const Eigen::Vector4d observer =
        parsePoint(
            screen["observer"],
            "screen.observer"
        );

    return Screen(
        dpi,
        dimensions,
        position,
        observer
    );
}

/**
 * @brief Parses the global optical properties of the scene medium.
 *
 * @param root Root JSON object containing the medium entry.
 * @return Parsed Medium object.
 *
 * @throws std::runtime_error If required medium parameters are missing
 * or contain invalid values.
 */
Medium parseMedium(const json& root) {
    if (!root.contains("medium") ||
        !root["medium"].is_object()) {
        throw std::runtime_error(
            "Scene must contain a valid 'medium' object."
        );
    }

    const json& medium = root["medium"];

    if (!medium.contains("ambient")) {
        throw std::runtime_error(
            "medium.ambient is required."
        );
    }

    if (!medium.contains("index") ||
        !medium["index"].is_number()) {
        throw std::runtime_error(
            "medium.index must be numeric."
        );
    }

    if (!medium.contains("recursion") ||
        !medium["recursion"].is_number_integer()) {
        throw std::runtime_error(
            "medium.recursion must be an integer."
        );
    }

    const Eigen::Vector3d ambient =
        parseRgb(
            medium["ambient"],
            "medium.ambient"
        );

    const double index =
        medium["index"].get<double>();

    const int recursion =
        medium["recursion"].get<int>();

    if (index <= 0.0) {
        throw std::runtime_error(
            "medium.index must be greater than zero."
        );
    }

    if (recursion < 0) {
        throw std::runtime_error(
            "medium.recursion cannot be negative."
        );
    }

    return Medium(
        ambient,
        index,
        recursion
    );
}

/**
 * @brief Parses one point light source.
 *
 * @param value JSON object describing one light source.
 * @param sourceIndex Index in the source array, used in error messages.
 * @return Parsed LightSource object.
 *
 * @throws std::runtime_error If required light-source parameters are
 * missing or malformed.
 */
LightSource parseLightSource(
    const json& value,
    std::size_t sourceIndex
) {
    const std::string prefix =
        "sources[" +
        std::to_string(sourceIndex) +
        "]";

    if (!value.is_object()) {
        throw std::runtime_error(
            prefix + " must be a JSON object."
        );
    }

    if (!value.contains("position")) {
        throw std::runtime_error(
            prefix + ".position is required."
        );
    }

    if (!value.contains("intensity")) {
        throw std::runtime_error(
            prefix + ".intensity is required."
        );
    }

    const Eigen::Vector4d position =
        parsePoint(
            value["position"],
            prefix + ".position"
        );

    const Eigen::Vector3d intensity =
        parseRgb(
            value["intensity"],
            prefix + ".intensity"
        );

    return LightSource(
        position,
        intensity
    );
}

/**
 * @brief Parses all point light sources contained in a scene.
 *
 * @param root Root JSON object containing the sources entry.
 * @return Collection of parsed light sources.
 *
 * @throws std::runtime_error If sources is missing, is not an array,
 * or contains malformed light sources.
 */
std::vector<LightSource> parseSources(
    const json& root
) {
    if (!root.contains("sources") ||
        !root["sources"].is_array()) {
        throw std::runtime_error(
            "Scene must contain a valid 'sources' array."
        );
    }

    const json& sourceArray =
        root["sources"];

    std::vector<LightSource> sources;
    sources.reserve(sourceArray.size());

    for (std::size_t i = 0;
         i < sourceArray.size();
         ++i) {
        sources.push_back(
            parseLightSource(
                sourceArray[i],
                i
            )
        );
    }

    return sources;
}

/**
 * @brief Parses the optical material parameters of a primitive object.
 *
 * Missing material coefficients use the defaults defined by the input
 * format: zero RGB coefficients and shininess equal to one.
 *
 * @param value JSON object containing material coefficients.
 * @param fieldName Name of the field, used for error messages.
 * @return Parsed Material object.
 *
 * @throws std::runtime_error If the material description is malformed.
 */
Material parseMaterial(
    const json& value,
    const std::string& fieldName
) {
    if (!value.is_object()) {
        throw std::runtime_error(
            fieldName +
            " must be a JSON object."
        );
    }

    Eigen::Vector3d ambient =
        Eigen::Vector3d::Zero();

    Eigen::Vector3d diffuse =
        Eigen::Vector3d::Zero();

    Eigen::Vector3d specular =
        Eigen::Vector3d::Zero();

    Eigen::Vector3d reflected =
        Eigen::Vector3d::Zero();

    Eigen::Vector3d refracted =
        Eigen::Vector3d::Zero();

    double shininess = 1.0;

    if (value.contains("ambient")) {
        ambient = parseRgb(
            value["ambient"],
            fieldName + ".ambient"
        );
    }

    if (value.contains("diffuse")) {
        diffuse = parseRgb(
            value["diffuse"],
            fieldName + ".diffuse"
        );
    }

    if (value.contains("specular")) {
        specular = parseRgb(
            value["specular"],
            fieldName + ".specular"
        );
    }

    if (value.contains("reflected")) {
        reflected = parseRgb(
            value["reflected"],
            fieldName + ".reflected"
        );
    }

    if (value.contains("refracted")) {
        refracted = parseRgb(
            value["refracted"],
            fieldName + ".refracted"
        );
    }

    if (value.contains("shininess")) {
        if (!value["shininess"].is_number()) {
            throw std::runtime_error(
                fieldName +
                ".shininess must be numeric."
            );
        }

        shininess = value["shininess"].get<double>();

        if (shininess < 0.0) {
            throw std::runtime_error(
                fieldName +
                ".shininess cannot be negative."
            );
        }
    }

    return Material(
        ambient,
        diffuse,
        specular,
        reflected,
        refracted,
        shininess
    );
}

/**
 * @brief Parses a sphere primitive.
 *
 * @param value JSON object containing sphere parameters.
 * @param fieldName Name used for descriptive error messages.
 * @return Constructed Sphere through the Object base type.
 *
 * @throws std::runtime_error If required parameters are missing or invalid.
 */
std::unique_ptr<Object> parseSphere(
    const json& value,
    const std::string& fieldName
) {
    if (!value.is_object()) {
        throw std::runtime_error(
            fieldName +
            " must be a JSON object."
        );
    }

    if (!value.contains("position")) {
        throw std::runtime_error(
            fieldName +
            ".position is required."
        );
    }

    if (!value.contains("radius") ||
        !value["radius"].is_number()) {
        throw std::runtime_error(
            fieldName +
            ".radius must be numeric."
        );
    }

    if (!value.contains("color")) {
        throw std::runtime_error(
            fieldName +
            ".color is required."
        );
    }

    if (!value.contains("index") ||
        !value["index"].is_number()) {
        throw std::runtime_error(
            fieldName +
            ".index must be numeric."
        );
    }

    const Eigen::Vector4d position =
        parsePoint(
            value["position"],
            fieldName + ".position"
        );

    const double radius =
        value["radius"].get<double>();

    if (radius <= 0.0) {
        throw std::runtime_error(
            fieldName +
            ".radius must be greater than zero."
        );
    }

    const Material material =
        parseMaterial(
            value["color"],
            fieldName + ".color"
        );

    const double index =
        value["index"].get<double>();

    if (index <= 0.0) {
        throw std::runtime_error(
            fieldName +
            ".index must be greater than zero."
        );
    }

    return std::make_unique<Sphere>(
        position,
        radius,
        material,
        index
    );
}

/**
 * @brief Parses a half-space primitive.
 *
 * The supplied normal may have any non-zero length. It is normalized
 * before the HalfSpace is constructed so downstream geometry and lighting
 * code can rely on receiving a unit normal.
 *
 * @param value JSON object containing half-space parameters.
 * @param fieldName Name used for descriptive error messages.
 * @return Constructed HalfSpace through the Object base type.
 *
 * @throws std::runtime_error If required parameters are missing or invalid.
 */
std::unique_ptr<Object> parseHalfSpace(
    const json& value,
    const std::string& fieldName
) {
    if (!value.is_object()) {
        throw std::runtime_error(
            fieldName +
            " must be a JSON object."
        );
    }

    if (!value.contains("position")) {
        throw std::runtime_error(
            fieldName +
            ".position is required."
        );
    }

    if (!value.contains("normal")) {
        throw std::runtime_error(
            fieldName +
            ".normal is required."
        );
    }

    if (!value.contains("color")) {
        throw std::runtime_error(
            fieldName +
            ".color is required."
        );
    }

    if (!value.contains("index") ||
        !value["index"].is_number()) {
        throw std::runtime_error(
            fieldName +
            ".index must be numeric."
        );
    }

    const Eigen::Vector4d position =
        parsePoint(
            value["position"],
            fieldName + ".position"
        );

    Eigen::Vector4d normal =
        parseDirection(
            value["normal"],
            fieldName + ".normal"
        );

    const double normalLength =
        normal.head<3>().norm();

    if (normalLength == 0.0) {
        throw std::runtime_error(
            fieldName +
            ".normal must not be the zero vector."
        );
    }

    normal.head<3>() /= normalLength;

    const Material material =
        parseMaterial(
            value["color"],
            fieldName + ".color"
        );

    const double index =
        value["index"].get<double>();

    if (index <= 0.0) {
        throw std::runtime_error(
            fieldName +
            ".index must be greater than zero."
        );
    }

    return std::make_unique<HalfSpace>(
        position,
        normal,
        material,
        index
    );
}

/**
 * @brief Parses a scaling transformation and its nested subject.
 *
 * @param value JSON object describing the transformation.
 * @return Constructed Scaling object.
 *
 * @throws std::runtime_error If parameters or the nested subject are invalid.
 */
std::unique_ptr<Object> parseScaling(
    const json& value
) {
    if (!value.is_object()) {
        throw std::runtime_error(
            "scaling must be a JSON object."
        );
    }

    if (!value.contains("factors")) {
        throw std::runtime_error(
            "scaling.factors is required."
        );
    }

    if (!value.contains("subject")) {
        throw std::runtime_error(
            "scaling.subject is required."
        );
    }

    const Eigen::Vector3d factors =
        parseVector3(
            value["factors"],
            "scaling.factors"
        );

    if (factors.x() == 0.0 ||
        factors.y() == 0.0 ||
        factors.z() == 0.0) {
        throw std::runtime_error(
            "scaling factors must not be zero."
        );
    }

    std::unique_ptr<Object> subject =
        parseObject(value["subject"]);

    return std::make_unique<Scaling>(
        factors,
        std::move(subject)
    );
}

/**
 * @brief Parses a translation transformation and its nested subject.
 *
 * @param value JSON object describing the transformation.
 * @return Constructed Translation object.
 *
 * @throws std::runtime_error If parameters or the nested subject are invalid.
 */
std::unique_ptr<Object> parseTranslation(
    const json& value
) {
    if (!value.is_object()) {
        throw std::runtime_error(
            "translation must be a JSON object."
        );
    }

    if (!value.contains("factors")) {
        throw std::runtime_error(
            "translation.factors is required."
        );
    }

    if (!value.contains("subject")) {
        throw std::runtime_error(
            "translation.subject is required."
        );
    }

    const Eigen::Vector3d factors =
        parseVector3(
            value["factors"],
            "translation.factors"
        );

    std::unique_ptr<Object> subject =
        parseObject(value["subject"]);

    return std::make_unique<Translation>(
        factors,
        std::move(subject)
    );
}

/**
 * @brief Parses a rotation transformation and its nested subject.
 *
 * @param value JSON object describing the transformation.
 * @return Constructed Rotation object.
 *
 * @throws std::runtime_error If parameters or the nested subject are invalid.
 */
std::unique_ptr<Object> parseRotation(
    const json& value
) {
    if (!value.is_object()) {
        throw std::runtime_error(
            "rotation must be a JSON object."
        );
    }

    if (!value.contains("angle") ||
        !value["angle"].is_number()) {
        throw std::runtime_error(
            "rotation.angle must be numeric."
        );
    }

    if (!value.contains("direction") ||
        !value["direction"].is_number_integer()) {
        throw std::runtime_error(
            "rotation.direction must be an integer."
        );
    }

    if (!value.contains("subject")) {
        throw std::runtime_error(
            "rotation.subject is required."
        );
    }

    const double angle =
        value["angle"].get<double>();

    const int direction =
        value["direction"].get<int>();

    RotationAxis axis;

    if (direction == 0) {
        axis = RotationAxis::X;
    }
    else if (direction == 1) {
        axis = RotationAxis::Y;
    }
    else if (direction == 2 ||
             direction == 3) {
        axis = RotationAxis::Z;
    }
    else {
        throw std::runtime_error(
            "rotation.direction must be 0, 1, 2, or 3."
        );
    }

    std::unique_ptr<Object> subject =
        parseObject(value["subject"]);

    return std::make_unique<Rotation>(
        angle,
        axis,
        std::move(subject)
    );
}

/**
 * @brief Parses the ordered child-object list of an object combination.
 *
 * Each array element is parsed recursively as a complete scene object.
 *
 * @param value JSON value expected to contain an array of objects.
 * @param fieldName Combination name, used for error messages.
 * @return Collection owning the recursively parsed child objects.
 *
 * @throws std::runtime_error If the value is not an array or contains
 * malformed object descriptions.
 */
std::vector<std::unique_ptr<Object>> parseCombinationChildren(
    const json& value,
    const std::string& fieldName
) {
    if (!value.is_array()) {
        throw std::runtime_error(
            fieldName + " must be an array of objects."
        );
    }

    std::vector<std::unique_ptr<Object>> children;
    children.reserve(value.size());

    for (const auto& child : value) {
        children.push_back(
            parseObject(child)
        );
    }

    return children;
}

/**
 * @brief Parses a recursive union of scene objects.
 *
 * @param value JSON array containing the union's child objects.
 * @return Constructed Union object.
 *
 * @throws std::runtime_error If the child-object list is malformed.
 */
std::unique_ptr<Object> parseUnion(
    const json& value
) {
    auto children =
        parseCombinationChildren(
            value,
            "union"
        );

    return std::make_unique<Union>(
        std::move(children)
    );
}

/**
 * @brief Parses a recursive intersection of scene objects.
 *
 * @param value JSON array containing the intersection's child objects.
 * @return Constructed Intersection object.
 *
 * @throws std::runtime_error If the child-object list is malformed.
 */
std::unique_ptr<Object> parseIntersection(
    const json& value
) {
    auto children =
        parseCombinationChildren(
            value,
            "intersection"
        );

    return std::make_unique<Intersection>(
        std::move(children)
    );
}

/**
 * @brief Parses an ordered difference of scene objects.
 *
 * Child order is preserved so that the resulting representation means
 * O1 - O2 - ... - ON.
 *
 * @param value JSON array containing the difference's child objects.
 * @return Constructed Difference object.
 *
 * @throws std::runtime_error If the child-object list is malformed.
 */
std::unique_ptr<Object> parseDifference(
    const json& value
) {
    auto children =
        parseCombinationChildren(
            value,
            "difference"
        );

    return std::make_unique<Difference>(
        std::move(children)
    );
}

/**
 * @brief Constructs one geometric object from its JSON representation.
 *
 * The function dispatches to the correct parser according to the single
 * object-type key. Transformation parsers recursively call this function
 * for their nested subjects.
 *
 * @param value JSON representation of one complete scene object.
 * @return Exclusive ownership of the constructed object.
 *
 * @throws std::runtime_error If the description is malformed or uses an
 * unsupported object type.
 */
std::unique_ptr<Object> parseObject(
    const json& value
) {
    if (!value.is_object() ||
        value.size() != 1) {
        throw std::runtime_error(
            "Each scene object must contain exactly one object type."
        );
    }

    const auto iterator =
        value.begin();

    const std::string type =
        iterator.key();

    const json& parameters =
        iterator.value();

    if (type == "sphere") {
        return parseSphere(parameters, "sphere");
    }

    if (type == "halfSpace") {
        return parseHalfSpace(parameters, "halfSpace");
    }

    if (type == "scaling") {
        return parseScaling(parameters);
    }

    if (type == "translation") {
        return parseTranslation(parameters);
    }

    if (type == "rotation") {
        return parseRotation(parameters);
    }

    if (type == "union") {
        return parseUnion(parameters);
    }

    if (type == "intersection") {
        return parseIntersection(parameters);
    }

    if (type == "difference") {
        return parseDifference(parameters);
    }

    throw std::runtime_error(
        "Unsupported object type: " + type
    );
}

/**
 * @brief Parses all geometric objects contained in a scene.
 *
 * @param root Root JSON object containing the objects entry.
 * @return Collection owning all parsed scene objects.
 *
 * @throws std::runtime_error If objects is missing, malformed, or contains
 * invalid object descriptions.
 */
std::vector<std::unique_ptr<Object>> parseObjects(
    const json& root
) {
    if (!root.contains("objects") ||
        !root["objects"].is_array()) {
        throw std::runtime_error(
            "Scene must contain a valid 'objects' array."
        );
    }

    const json& objectArray =
        root["objects"];

    std::vector<std::unique_ptr<Object>> objects;
    objects.reserve(objectArray.size());

    for (const auto& object : objectArray) {
        objects.push_back(
            parseObject(object)
        );
    }

    return objects;
}

} // namespace

Scene SceneParser::load(
    const std::string& filename
) {
    std::ifstream input(filename);

    if (!input.is_open()) {
        throw std::runtime_error(
            "Could not open scene file: " +
            filename
        );
    }

    json root;

    try {
        input >> root;
    }
    catch (const json::parse_error& error) {
        throw std::runtime_error(
            "Could not parse JSON scene file '" +
            filename +
            "': " +
            error.what()
        );
    }

    if (!root.is_object()) {
        throw std::runtime_error(
            "The root of the scene file must be a JSON object."
        );
    }

    const Screen screen =
        parseScreen(root);

    const Medium medium =
        parseMedium(root);

    const std::vector<LightSource> sources =
        parseSources(root);

    std::vector<std::unique_ptr<Object>> objects =
        parseObjects(root);

    return Scene(
        screen,
        medium,
        sources,
        std::move(objects)
    );
}
