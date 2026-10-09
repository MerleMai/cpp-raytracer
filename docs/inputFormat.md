# JSON Scene Input Format

This document describes the JSON accepted by `SceneParser::load()`. It is the
input contract for developers creating scene files and for code consuming the
resulting `Scene`.

## Complete top-level structure

The root must be a JSON object containing all four entries shown below:

```json
{
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
    "objects": []
}
```

| Entry | Meaning | May be empty? |
| --- | --- | --- |
| `screen`  | Screen surface and observer configuration | No |
| `medium`  | Global ambient and optical settings       | No |
| `sources` | Array of point light sources              | Yes |
| `objects` | Array of root geometric objects           | Yes |

## Common representations

### Points and directions

JSON stores three Cartesian components. The parser converts them to Eigen
homogeneous coordinates:

| Meaning   | JSON        | Internal value |
| --- | --- | --- |
| Point     | `[x, y, z]` | `(x, y, z, 1)` |
| Direction | `[x, y, z]` | `(x, y, z, 0)` |

### RGB values

RGB values are arrays of exactly three numbers in the range `[0, 1]`:

```json
[0.25, 0.5, 1.0]
```

## Screen

```json
"screen": {
    "dpi": 128,
    "dimensions": [2, 4],
    "position": [-1, -2, -10],
    "observer": [0, 0, -20]
}
```

| Field        | Type          | Meaning |
| --- | --- | --- |
| `dpi`        | integer       | Pixels per length unit in both screen directions |
| `dimensions` | two numbers   | Physical width and height |
| `position`   | three numbers | Bottom-left point of the screen surface |
| `observer`   | three numbers | Observer position |

Validation:

- `dpi` must be greater than zero
- both dimensions must be greater than zero
- `dimensions` must contain exactly two numbers
- `position` and `observer` must each contain exactly three numbers

## Medium

```json
"medium": {
    "ambient": [0.1, 0.1, 0.1],
    "index": 1.0,
    "recursion": 1
}
```

| Field       | Type    | Meaning |
| --- | --- | --- |
| `ambient`   | RGB     | Global ambient-light intensity |
| `index`     | number  | Refractive index of the surrounding medium |
| `recursion` | integer | Maximum reflection/refraction recursion depth |

Validation:

- every ambient component must be in `[0, 1]`
- `index` must be greater than zero
- `recursion` must be greater than or equal to zero

## Light sources

`sources` is an array. Each entry represents one point light:

```json
"sources": [
    {
        "position": [0, 0, 0],
        "intensity": [1, 1, 1]
    },
    {
        "position": [2, 1, -3],
        "intensity": [0.5, 0.4, 0.3]
    }
]
```

| Field       | Type          | Meaning |
| --- | --- | --- |
| `position`  | three numbers | Position converted to `(x, y, z, 1)` |
| `intensity` | RGB           | Light intensity for red, green, and blue |

The array may be empty. Every intensity component must be in `[0, 1]`.

## Objects and recursive nesting

`objects` is an array of root objects. Every object description must contain
exactly one key identifying its type:

```json
{
    "sphere": {
        "position": [0, 0, 0],
        "radius": 1,
        "color": {},
        "index": 1
    }
}
```

Supported keys:

- primitives: `sphere`, `halfSpace`
- transformations: `scaling`, `translation`, `rotation`
- combinations: `union`, `intersection`, `difference`

Transformations and combinations contain complete object descriptions, so
objects may be nested to arbitrary depth.

## Materials

Primitive objects contain a `color` object with these optional fields:

| Field | Type | Default | Meaning |
| --- | --- | --- | --- |
| `ambient`   | RGB    | `[0, 0, 0]` | Ambient reflection coefficient |
| `diffuse`   | RGB    | `[0, 0, 0]` | Diffuse reflection coefficient |
| `specular`  | RGB    | `[0, 0, 0]` | Specular reflection coefficient |
| `reflected` | RGB    | `[0, 0, 0]` | Reflected-light coefficient |
| `refracted` | RGB    | `[0, 0, 0]` | Refracted-light coefficient |
| `shininess` | number | `1`         | Phong shininess coefficient |

An empty object selects every default:

```json
"color": {}
```

Every supplied RGB component must be in `[0, 1]`. `shininess` must be greater
than or equal to zero.

## Primitive objects

### Sphere

```json
{
    "sphere": {
        "position": [0, 0, 0],
        "radius": 1.0,
        "color": {
            "ambient": [0.1, 0.1, 0.1],
            "diffuse": [1, 0, 0],
            "specular": [1, 1, 1],
            "shininess": 10
        },
        "index": 1.5
    }
}
```

| Field | Requirement |
| --- | --- |
| `position` | Point containing exactly three numbers |
| `radius`   | Number greater than zero |
| `color`    | Material object; individual material fields are optional |
| `index`    | Refractive index greater than zero |

### Half-space

```json
{
    "halfSpace": {
        "position": [0, -1, 0],
        "normal": [0, 2, 0],
        "color": {},
        "index": 1.0
    }
}
```

| Field | Requirement |
| --- | --- |
| `position` | Point on the boundary plane |
| `normal`   | Non-zero direction vector |
| `color`    | Material object |
| `index`    | Refractive index greater than zero |

The input normal does not need to have unit length. Before constructing the
`HalfSpace`, the parser converts it to homogeneous coordinates and normalizes
its first three components. In the example, `[0, 2, 0]` is stored internally as
`(0, 1, 0, 0)`.

## Transformations

Every transformation contains one complete object in `subject`.

### Scaling

```json
{
    "scaling": {
        "factors": [2, 3, 4],
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
```

`factors` contains scaling along x, y, and z. All three values must be non-zero
because later intersection calculations require an inverse transformation.

### Translation

```json
{
    "translation": {
        "factors": [1, 2, 3],
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
```

`factors` contains translation along x, y, and z.

### Rotation

```json
{
    "rotation": {
        "angle": 90,
        "direction": 2,
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
```

- `angle` is numeric and expressed in degrees
- `direction = 0` selects the x-axis
- `direction = 1` selects the y-axis
- `direction = 2` selects the z-axis
- `direction = 3` is accepted as a compatibility value for the z-axis

## Object combinations

Combinations contain an array of complete child objects.

### Union

```json
{
    "union": [
        {
            "sphere": {
                "position": [0, 0, 0],
                "radius": 1,
                "color": {},
                "index": 1
            }
        },
        {
            "sphere": {
                "position": [1, 0, 0],
                "radius": 1,
                "color": {},
                "index": 1
            }
        }
    ]
}
```

### Intersection

`intersection` has the same recursive array structure as `union`.

### Difference

The child order is significant:
O1 - O2 - ... - ON

The parser validates that every combination value is an array and that every
element is a valid object. During rendering, each CSG class filters and sorts
the intersections from its children according to the selected operation.

## Error handling

The parser throws `std::runtime_error` when:

- the file cannot be opened
- the file is not valid JSON
- the root or a required section has the wrong type
- a required field is missing
- a vector has the wrong size or contains non-numeric values
- a numeric value violates a documented constraint
- an object has zero or multiple type keys
- an object type is unsupported
- a recursively nested subject or child is invalid

Error messages include the relevant field or object type whenever possible.

## Working examples

- [`basicScene.json`](../examples/basicScene.json): screen, medium, and lights
- [`primitiveScene.json`](../examples/primitiveScene.json): sphere and half-space
- [`transformationScene.json`](../examples/transformationScene.json): nested transformations
- [`combinationScene.json`](../examples/combinationScene.json): nested CSG tree
