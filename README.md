# nfx-serialization

<!-- Project Info -->

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://github.com/nfx-libs/nfx-serialization/blob/main/LICENSE.txt) [![GitHub release (latest by date)](https://img.shields.io/github/v/release/nfx-libs/nfx-serialization?style=flat-square)](https://github.com/nfx-libs/nfx-serialization/releases) [![GitHub tag (latest by date)](https://img.shields.io/github/tag/nfx-libs/nfx-serialization?style=flat-square)](https://github.com/nfx-libs/nfx-serialization/tags)<br/>

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square) ![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg?style=flat-square) ![Cross Platform](https://img.shields.io/badge/Platform-Linux_Windows-lightgrey?style=flat-square)

<!-- CI/CD Status -->

[![Linux GCC](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/build-linux-gcc.yml?branch=main&label=Linux%20GCC&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/build-linux-gcc.yml) [![Linux Clang](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/build-linux-clang.yml?branch=main&label=Linux%20Clang&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/build-linux-clang.yml) [![Windows MinGW](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/build-windows-mingw.yml?branch=main&label=Windows%20MinGW&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/build-windows-mingw.yml) [![Windows MSVC](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/build-windows-msvc.yml?branch=main&label=Windows%20MSVC&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/build-windows-msvc.yml) [![CodeQL](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/codeql.yml?branch=main&label=CodeQL&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/codeql.yml)

> A cross-platform C++20 library providing extensible C++ type serialization on top of nfx-json

## Overview

nfx-serialization is a modern C++20 library that provides a powerful C++ type serialization layer built on top of the [nfx-json](https://github.com/nfx-libs/nfx-json) library. It offers automatic serialization and deserialization of C++ types to/from JSON through an extensible trait system, supporting STL containers, smart pointers, optional types, and custom user-defined types - all optimized for performance across multiple platforms and compilers.

## Key Features

### 🔄 C++ Type Serialization

- **Serializer<T>**: Template-based automatic serialization/deserialization with compile-time type detection
- **SerializationTraits**: Extensible trait system for custom type support with asymmetric read/write
  - **Write (serialize)**: Streaming JSON generation via Builder API (zero DOM overhead)
  - **Read (fromDocument)**: DOM-based deserialization via Document API (type-safe navigation)
- **Type-safe**: Automatic type mapping with compile-time verification
- **Zero-overhead**: Header-only template implementations with inline expansion and SFINAE-based dispatch

### 📦 Supported Types

- POD types (integers, floats, booleans, strings)
- STL containers (`vector`, `array`, `list`, `deque`, `set`, `unordered_set`, `map`, `unordered_map`)
- Smart pointers (`unique_ptr`, `shared_ptr`)
- Optional types (`std::optional`, `std::nullopt`)
- Custom types via `SerializationTraits` specialization
- Nested structures and containers

### 🔌 Optional nfx Library Extensions

- **nfx-containers**: `FastHashMap`, `FastHashSet`, `PerfectHashMap`
- **nfx-datatypes**: `Int128`, `Decimal`
- **nfx-datetime**: `DateTime`, `DateTimeOffset`, `TimeSpan`

### 🏗️ Architecture

nfx-serialization is built on [nfx-json](https://github.com/nfx-libs/nfx-json), which provides:
- **Document**: Variant-based JSON document with type-safe manipulation
- **Builder**: Streaming JSON generation API (SAX-like, single-pass, optimal performance)
- **SchemaValidator**: JSON Schema Draft 2020-12 validation
- **SchemaGenerator**: Automatic schema inference from samples
- **PathView**: Zero-copy traversal with JSON Pointer support

### 📊 Real-World Applications

- **Configuration Management**: Serialize app settings and configuration objects
- **API Integration**: Automatic serialization for REST/GraphQL request/response
- **Data Persistence**: Save/load C++ objects to/from JSON files
- **IPC & Messaging**: Serialize C++ structures for inter-process communication
- **Game Development**: Save game state, player data, inventory systems
- **Database Integration**: Convert C++ objects for document databases (MongoDB, etc.)
- **Logging Systems**: Structured logging with automatic JSON formatting
- **Testing & Mocking**: Easy test data generation and comparison

### 🌍 Cross-Platform Support

- Linux, Windows
- GCC 14+, Clang 18+, MSVC 2022+
- Thread-safe operations
- Consistent behavior across platforms

## Quick Start

### Requirements

- C++20 compatible compiler:
  - **GCC 14+** (14.2.0 tested)
  - **Clang 18+** (19.1.7 tested)
  - **MSVC 2022+** (19.44+ tested)
- CMake 3.20 or higher

### CMake Integration

```cmake
# --- JSON serialization support ---
option(NFX_SERIALIZATION_WITH_JSON             "Enable JSON serialization support"  ON )

# --- Build components ---
option(NFX_SERIALIZATION_BUILD_TESTS           "Build tests"                        OFF)
option(NFX_SERIALIZATION_BUILD_EXTENSION_TESTS "Build extension tests"              OFF)
option(NFX_SERIALIZATION_BUILD_SAMPLES         "Build samples"                      OFF)
option(NFX_SERIALIZATION_BUILD_BENCHMARKS      "Build benchmarks"                   OFF)
option(NFX_SERIALIZATION_BUILD_DOCUMENTATION   "Build Doxygen documentation"        OFF)

# --- Installation ---
option(NFX_SERIALIZATION_INSTALL_PROJECT       "Install project"                    OFF)

# --- Packaging ---
option(NFX_SERIALIZATION_PACKAGE_SOURCE        "Enable source package generation"   OFF)
option(NFX_SERIALIZATION_PACKAGE_ARCHIVE       "Enable TGZ/ZIP package generation"  OFF)
option(NFX_SERIALIZATION_PACKAGE_DEB           "Enable DEB package generation"      OFF)
option(NFX_SERIALIZATION_PACKAGE_RPM           "Enable RPM package generation"      OFF)
option(NFX_SERIALIZATION_PACKAGE_WIX           "Enable WiX Windows installer (MSI)" OFF)
```

### Using in Your Project

#### Option 1: Using FetchContent (Recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
  nfx-serialization
  GIT_REPOSITORY https://github.com/nfx-libs/nfx-serialization.git
  GIT_TAG        main  # or use specific version tag like "0.5.0"
)
FetchContent_MakeAvailable(nfx-serialization)

# Link with header-only library
target_link_libraries(your_target PRIVATE nfx-serialization::nfx-serialization)
```

#### Option 2: As a Git Submodule

```bash
# Add as submodule
git submodule add https://github.com/nfx-libs/nfx-serialization.git third-party/nfx-serialization
```

```cmake
# In your CMakeLists.txt
add_subdirectory(third-party/nfx-serialization)
target_link_libraries(your_target PRIVATE nfx-serialization::nfx-serialization)
```

#### Option 3: Using find_package (After Installation)

```cmake
find_package(nfx-serialization REQUIRED)
target_link_libraries(your_target PRIVATE nfx-serialization::nfx-serialization)
```

### Building

**Build Commands:**

```bash
# Clone the repository
git clone https://github.com/nfx-libs/nfx-serialization.git
cd nfx-serialization

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build . --config Release --parallel

# Run tests (optional)
ctest -C Release --output-on-failure

# Run benchmarks (optional)
./bin/Release/BM_JSON_Serialization
```

### Documentation

nfx-serialization includes API documentation generated with Doxygen.

#### 📚 Online Documentation

The complete API documentation is available online at:
**https://nfx-libs.github.io/nfx-serialization**

#### Building Documentation Locally

```bash
# Configure with documentation enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DNFX_SERIALIZATION_BUILD_DOCUMENTATION=ON

# Build the documentation
cmake --build . --target nfx-serialization-documentation
```

#### Requirements

- **Doxygen** - Documentation generation tool
- **Graphviz Dot** (optional) - For generating class diagrams

#### Accessing Local Documentation

After building, open `./build/doc/html/index.html` in your web browser.

### Sample Programs

nfx-serialization includes tutorial samples demonstrating progressive JSON serialization workflows. Each sample is self-contained and follows a consistent educational format with numbered sections, inline validation, and formatted output.

**Available samples:**
- **Sample_JsonSerializationBasics.cpp** - Document and Builder API fundamentals
- **Sample_JsonSerializationContainers.cpp** - Automatic STL container serialization
- **Sample_JsonSerializationTraits.cpp** - Custom type serialization with SerializationTraits

See [samples/README.md](samples/README.md) for detailed descriptions and learning path.

**Building and running samples:**

```bash
# Configure with samples enabled
cmake -B build -DCMAKE_BUILD_TYPE=Release -DNFX_SERIALIZATION_BUILD_SAMPLES=ON

# Build all samples
cmake --build build --config Release

# Run samples
./build/bin/Sample_JsonSerializationBasics
./build/bin/Sample_JsonSerializationContainers
./build/bin/Sample_JsonSerializationTraits
```

## Usage Examples

### Basic Serialization - STL Containers

```cpp
#include <nfx/Serialization.h>
#include <vector>
#include <map>

using namespace nfx::serialization::json;

// Serialize STL containers to JSON
std::vector<int> numbers = {1, 2, 3, 4, 5};
std::string json = Serializer<std::vector<int>>::toString(numbers);
// Result: "[1,2,3,4,5]"

// Deserialize JSON back to STL containers
std::vector<int> restored = Serializer<std::vector<int>>::fromString(json);
// restored == numbers

// Map example
std::map<std::string, int> scores;
scores["Alice"] = 95;
scores["Bob"] = 87;
std::string scoresJson = Serializer<decltype(scores)>::toString(scores);
// Result: {"Alice":95,"Bob":87}

// Pretty-print with options
Serializer<std::vector<int>>::Options opts;
opts.prettyPrint = true;
std::string prettyJson = Serializer<std::vector<int>>::toString(numbers, opts);
```

### Working with JSON Documents (from nfx-json)

```cpp
#include <nfx/Serialization.h>

using namespace nfx::json;                 // Document types
using namespace nfx::serialization::json;  // Serializer

// Parse JSON string into Document
auto docOpt = Document::fromString(R"({
    "name": "John Doe",
    "age": 30,
    "hobbies": ["reading", "gaming"]
})");

if (!docOpt) {
    std::cerr << "Invalid JSON" << std::endl;
    return 1;
}

Document& doc = *docOpt;

// Read values with type safety
auto name = doc.get<std::string>("/name");  // optional<string>
auto age = doc.get<int64_t>("/age");        // optional<int64_t>

// Extract array and deserialize to STL vector
auto hobbiesDoc = doc.get<Document>("/hobbies");
if (hobbiesDoc) {
    std::string hobbiesJson = hobbiesDoc->toString();
    auto hobbies = Serializer<std::vector<std::string>>::fromString(hobbiesJson);
    // hobbies == {"reading", "gaming"}
}
```

### Custom Type Serialization with SerializationTraits

```cpp
#include <nfx/Serialization.h>

using namespace nfx::json;
using namespace nfx::serialization::json;

// Custom data structure
struct Person {
    std::string name;
    int age;
    std::vector<std::string> hobbies;
};

// Define serialization traits with asymmetric read/write
template<>
struct SerializationTraits<Person> {
    // Streaming serialization (write) - uses Builder API for optimal performance
    static void serialize(const Person& person, Builder& builder) {
        builder.startObject();
        
        builder.key("name");
        builder.string(person.name);
        
        builder.key("age");
        builder.number(person.age);
        
        builder.key("hobbies");
        Serializer<std::vector<std::string>>::serializeValue(person.hobbies, builder);
        
        builder.endObject();
    }

    // DOM-based deserialization (read) - uses Document API for type-safe navigation
    static void fromDocument(const Document& doc, Person& person) {
        person.name = doc.get<std::string>("/name").value_or("");
        person.age = static_cast<int>(doc.get<int64_t>("/age").value_or(0));
        
        // Deserialize hobbies vector
        if (auto hobbiesDoc = doc.get<Document>("/hobbies")) {
            person.hobbies = Serializer<std::vector<std::string>>().deserialize(*hobbiesDoc);
        }
    }
};

// Usage - works just like STL types
Person alice;
alice.name = "Alice";
alice.age = 30;
alice.hobbies = {"reading", "coding"};

// Serialization uses streaming Builder
std::string json = Serializer<Person>::toString(alice);
// Result: {"name":"Alice","age":30,"hobbies":["reading","coding"]}

// Deserialization uses Document for type-safe navigation
Person restored = Serializer<Person>::fromString(json);
// restored == alice
```

### Complete Example - Combining nfx-json and nfx-serialization

```cpp
#include <nfx/Serialization.h>

int main()
{
    using namespace nfx::json;
    using namespace nfx::serialization::json;

    // Create a JSON document (nfx-json)
    Document doc;
    doc.set<std::string>("/title", "My Application");
    doc.set<std::string>("/version", "1.0.0");

    // Serialize STL vector (nfx-serialization)
    std::vector<std::string> users = {"Alice", "Bob", "Charlie"};
    std::string usersJson = Serializer<std::vector<std::string>>::toString(users);
    
    // Parse and add to document (nfx-json)
    auto usersDoc = Document::fromString(usersJson);
    if (usersDoc) {
        doc.set<Document>("/users", *usersDoc);
    }

    // Add nested configuration (nfx-json)
    doc.set<bool>("/config/debug", true);
    doc.set<int64_t>("/config/timeout", 30);

    // Print JSON (nfx-json)
    std::string json = doc.toString(2);
    std::cout << "Generated JSON:\n" << json << std::endl;

    // Iterate array (nfx-json)
    auto usersArrayOpt = doc.get<Array>("/users");
    if (usersArrayOpt) {
        std::cout << "\nUsers:" << std::endl;
        for (const auto& userDoc : usersArrayOpt.value()) {
            auto user = userDoc.get<std::string>("");
            if (user) {
                std::cout << "  - " << *user << std::endl;
            }
        }
    }

    // Deserialize back to STL vector (nfx-serialization)
    auto usersDocOpt = doc.get<Document>("/users");
    if (usersDocOpt) {
        auto restored = Serializer<std::vector<std::string>>::fromString(
            usersDocOpt->toString()
        );
        std::cout << "\nRestored " << restored.size() << " users" << std::endl;
    }

    return 0;
}
```

**Output:**

```
Generated JSON:
{
  "title": "My Application",
  "version": "1.0.0",
  "users": [
    "Alice",
    "Bob",
    "Charlie"
  ],
  "config": {
    "debug": true,
    "timeout": 30
  }
}

Users:
  - Alice
  - Bob
  - Charlie

Restored 3 users
```

### Custom Traits - Extending for Your Types

nfx-serialization provides two approaches for integrating custom types:

#### Approach 1: SerializationTraits for Custom Objects

For types that need custom JSON representation, implement `SerializationTraits` with asymmetric read/write:

```cpp
#include <nfx/Serialization.h>

using namespace nfx::json;
using namespace nfx::serialization::json;

struct Point3D {
    double x, y, z;
};

// Define how Point3D serializes (asymmetric read/write architecture)
template<>
struct SerializationTraits<Point3D> {
    // Streaming serialization - uses Builder for optimal performance
    static void serialize(const Point3D& point, Builder& builder) {
        builder.startObject();
        builder.key("x");
        builder.number(point.x);
        builder.key("y");
        builder.number(point.y);
        builder.key("z");
        builder.number(point.z);
        builder.endObject();
    }

    // DOM-based deserialization - uses Document for type-safe navigation
    static void fromDocument(const Document& doc, Point3D& point) {
        point.x = doc.get<double>("/x").value_or(0.0);
        point.y = doc.get<double>("/y").value_or(0.0);
        point.z = doc.get<double>("/z").value_or(0.0);
    }
};

// Usage - works just like STL types
Point3D origin{0, 0, 0};
std::string json = Serializer<Point3D>::toString(origin);
// Result: {"x":0.0,"y":0.0,"z":0.0}

Point3D restored = Serializer<Point3D>::fromString(json);
```

#### Approach 2: Trait Specialization for Custom Containers

If you have a custom container that implements a standard container interface, mark it as a container so the Serializer automatically handles it:

```cpp
#include <nfx/Serialization.h>

// Your custom container
template<typename K, typename V>
class CustomHashMap {
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = std::pair<const K, V>;
    
    V& operator[](const K& key);
    auto begin() -> /* iterator */;
    auto end() -> /* iterator */;
    // ... standard container interface
};

// Mark it as a container (in detail namespace)
namespace nfx::serialization::json::detail {
    template<typename K, typename V>
    struct is_container<CustomHashMap<K,V>> : std::true_type {};
}

// Now it works automatically
CustomHashMap<std::string, int> myMap;
myMap["answer"] = 42;
std::string json = Serializer<CustomHashMap<std::string, int>>::toString(myMap);
// Result: {"answer":42}
```

**Required Container Interface:**

For **sequence containers** (vector-like):
- `using value_type = T;`
- `begin()` / `end()` iterators
- `insert(const value_type&)` for deserialization

For **associative containers** (map-like):
- `using key_type = K;`, `using mapped_type = V;`
- `using value_type = std::pair<const K, V>;`
- `operator[](const key_type&)` for deserialization
- `begin()` / `end()` iterators

**Note**: See `samples/Sample_JsonSerializer.cpp` for complete working examples of both approaches.

## Optional Extensions

nfx-serialization provides optional integration headers for other nfx libraries. These headers use conditional compilation (`__has_include()`) and are safe to include even if the external library is not installed:

### Available Extensions

- **`extensions/ContainersTraits.h`** - Serialization support for nfx-containers types
  - `nfx::containers::PerfectHashMap` - Compile-time perfect hash map
  - `nfx::containers::FastHashMap`    - Runtime open-addressing hash map
  - `nfx::containers::FastHashSet`    - Runtime open-addressing hash set
- **`extensions/DatatypesTraits.h`**  - Serialization support for nfx-datatypes types
  - `nfx::datatypes::Int128`          - 128-bit integer (cross-platform)
  - `nfx::datatypes::Decimal`         - Fixed-point decimal arithmetic
- **`extensions/DateTimeTraits.h`**   - Serialization support for nfx-datetime types
  - `nfx::time::DateTime`             - Date and time representation (ISO 8601)
  - `nfx::time::DateTimeOffset`       - Date and time with timezone offset
  - `nfx::time::TimeSpan`             - Duration/time interval

### Usage

```cpp
#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/extensions/ContainersTraits.h>  // Optional
#include <nfx/serialization/json/extensions/DateTimeTraits.h>    // Optional

#include <nfx/containers/FastHashMap.h>   // Requires nfx-containers
#include <nfx/datetime/DateTime.h>        // Requires nfx-datetime

using namespace nfx::serialization::json;

// Serialize nfx types just like STL types
nfx::containers::FastHashMap<std::string, int> scores;
scores.insertOrAssign("Alice", 95);
scores.insertOrAssign("Bob", 87);

std::string json = Serializer<decltype(scores)>::toString(scores);

// DateTime serialization
nfx::time::DateTime now = nfx::time::DateTime::now();
std::string timeJson = Serializer<nfx::time::DateTime>::toString(now);
// Result: "2025-11-30T10:30:45.123Z" (ISO 8601)
```

**Note**: These extensions are header-only and zero-cost - if you don't include them or don't have the external library installed, they have no impact on compile time or binary size.

## Installation & Packaging

nfx-serialization provides packaging options for distribution.

### Package Generation

```bash
# Configure with packaging options
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNFX_SERIALIZATION_PACKAGE_ARCHIVE=ON \
         -DNFX_SERIALIZATION_PACKAGE_DEB=ON \
         -DNFX_SERIALIZATION_PACKAGE_RPM=ON

# Generate binary packages
cmake --build . --target package
# or
cd build && cpack

# Generate source packages
cd build && cpack --config CPackSourceConfig.cmake
```

### Supported Package Formats

| Format      | Platform       | Description                        | Requirements |
| ----------- | -------------- | ---------------------------------- | ------------ |
| **TGZ/ZIP** | Cross-platform | Compressed archive packages        | None         |
| **DEB**     | Debian/Ubuntu  | Native Debian packages             | `dpkg-dev`   |
| **RPM**     | RedHat/SUSE    | Native RPM packages                | `rpm-build`  |
| **WiX**     | Windows        | Professional MSI installer         | `WiX 3.11+`  |
| **Source**  | Cross-platform | Source code distribution (TGZ+ZIP) | None         |

### Installation

```bash
# Linux (DEB-based systems)
sudo dpkg -i nfx-serialization_*_amd64.deb

# Linux (RPM-based systems)
sudo rpm -ivh nfx-serialization-*-Linux.rpm

# Windows (MSI installer)
nfx-serialization-0.1.0-MSVC.msi

# Manual installation (extract archive)
tar -xzf nfx-serialization-*-Linux.tar.gz -C /usr/local/
```

## Project Structure

```
nfx-serialization/
├── benchmark/                     # Performance benchmarks
├── cmake/                         # CMake configuration modules
├── include/nfx/
│   └── serialization/json/
│       ├── Serializer.h           # Main serializer class
│       ├── SerializationTraits.h  # Trait specialization interface (asymmetric read/write)
│       ├── Concepts.h             # C++20 concepts and type traits
│       └── extensions/            # Optional nfx library integrations
│           ├── ContainersTraits.h # nfx-containers support (FastHashMap, etc.)
│           ├── DatatypesTraits.h  # nfx-datatypes support (Int128, Decimal)
│           └── DateTimeTraits.h   # nfx-datetime support (DateTime, DateTimeOffset, TimeSpan)
├── samples/                       # Example code and demonstrations
└── test/                          # Unit tests with GoogleTest
```

**Note**: JSON core functionality (Document, SchemaValidator, SchemaGenerator, PathView) is provided by [nfx-json](https://github.com/nfx-libs/nfx-json), which is automatically fetched as a dependency.

## Performance

For detailed performance metrics and benchmarks, see the [benchmark documentation](benchmark/README.md).

## Roadmap

See [TODO.md](TODO.md) for upcoming features and project roadmap.

## Changelog

See the [CHANGELOG.md](CHANGELOG.md) for a detailed history of changes, new features, and bug fixes.

## License

This project is licensed under the MIT License.

## Dependencies

### Core Dependencies

- **[nfx-json](https://github.com/nfx-libs/nfx-json)**: JSON library with Document, SchemaValidator, and SchemaGenerator (MIT License)

### Development Dependencies

- **[GoogleTest](https://github.com/google/googletest)**: Testing framework (BSD 3-Clause License) - Development only
- **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework (Apache 2.0 License) - Development only

All dependencies are automatically fetched via CMake FetchContent when building the library, tests, or benchmarks.

---

_Updated on February 01, 2026_
