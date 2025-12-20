# nfx-serialization

<!-- Project Info -->

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://github.com/nfx-libs/nfx-serialization/blob/main/LICENSE.txt) [![GitHub release (latest by date)](https://img.shields.io/github/v/release/nfx-libs/nfx-serialization?style=flat-square)](https://github.com/nfx-libs/nfx-serialization/releases) [![GitHub tag (latest by date)](https://img.shields.io/github/tag/nfx-libs/nfx-serialization?style=flat-square)](https://github.com/nfx-libs/nfx-serialization/tags)<br/>

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square) ![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg?style=flat-square) ![Cross Platform](https://img.shields.io/badge/Platform-Linux_Windows-lightgrey?style=flat-square)

<!-- CI/CD Status -->

[![Linux GCC](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/build-linux-gcc.yml?branch=main&label=Linux%20GCC&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/build-linux-gcc.yml) [![Linux Clang](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/build-linux-clang.yml?branch=main&label=Linux%20Clang&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/build-linux-clang.yml) [![Windows MinGW](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/build-windows-mingw.yml?branch=main&label=Windows%20MinGW&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/build-windows-mingw.yml) [![Windows MSVC](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/build-windows-msvc.yml?branch=main&label=Windows%20MSVC&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/build-windows-msvc.yml) [![CodeQL](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-serialization/codeql.yml?branch=main&label=CodeQL&style=flat-square)](https://github.com/nfx-libs/nfx-serialization/actions/workflows/codeql.yml)

> A cross-platform C++20 JSON extensible serialization library with type-safe manipulation, automatic serialization, and schema validation

## Overview

nfx-serialization is a modern C++20 library providing comprehensive JSON serialization and deserialization capabilities. It offers type-safe document manipulation, automatic type mapping through an extensible trait system, STL-compatible iterators for arrays and objects, and JSON Schema validation - all optimized for performance across multiple platforms and compilers.

## Key Features

### 📦 Core JSON Components

- **Document**: Generic JSON document abstraction with type-safe value access and manipulation
- **Serializer**: Template-based automatic serialization/deserialization with extensible trait customization
- **SchemaValidator**: JSON Schema Draft 2020-12 validation with detailed error reporting
- **SchemaGenerator**: Automatic JSON Schema generation from sample documents with format detection

### 🔄 Automatic Type Mapping

- POD types (integers, floats, booleans, strings)
- STL containers (`vector`, `array`, `list`, `deque`, `set`, `unordered_set`, `map`, `unordered_map`)
- Smart pointers (`unique_ptr`, `shared_ptr`)
- Optional types (`std::optional`, `std::nullopt`)
- Custom types via `SerializationTraits` specialization
- Nested structures and containers

### ✅ JSON Schema Validation

- JSON Schema Draft 2020-12 support
- Comprehensive constraint validation (type, required, properties, etc.)
- Detailed error reporting with JSON Pointer paths
- Custom error messages and validation contexts

### 📊 Real-World Applications

- Configuration management (app settings, environment configs)
- API request/response handling (REST, GraphQL)
- Data persistence and caching
- Inter-process communication (IPC)
- Log processing and analysis
- Database document storage (NoSQL, MongoDB-style)
- Game save states and player data
- Message queue payloads (Kafka, RabbitMQ)

### ⚡ Performance Optimized

- Zero-copy document navigation with JSON Pointers
- STL-compatible iterators for arrays and objects with range-for support
- Compile-time type detection and optimization
- Header-only template implementations

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

# --- Library build types ---
option(NFX_SERIALIZATION_BUILD_STATIC          "Build static library"               OFF)
option(NFX_SERIALIZATION_BUILD_SHARED          "Build shared library"               OFF)

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
  GIT_TAG        main  # or use specific version tag like "0.1.0"
)
FetchContent_MakeAvailable(nfx-serialization)

# Link with static library
target_link_libraries(your_target PRIVATE nfx-serialization::static)
```

#### Option 2: As a Git Submodule

```bash
# Add as submodule
git submodule add https://github.com/nfx-libs/nfx-serialization.git third-party/nfx-serialization
```

```cmake
# In your CMakeLists.txt
add_subdirectory(third-party/nfx-serialization)
target_link_libraries(your_target PRIVATE nfx-serialization::static)
```

#### Option 3: Using find_package (After Installation)

```cmake
find_package(nfx-serialization REQUIRED)
target_link_libraries(your_target PRIVATE nfx-serialization::static)
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

## Usage Examples

### Document - JSON Manipulation

```cpp
#include <nfx/serialization/json/Document.h>

using namespace nfx::serialization::json;

// Create and manipulate JSON document
Document doc;

// Set values using JSON Pointer notation
doc.set<std::string>("/name", "John Doe");
doc.set<int>("/age", 30);
doc.set<std::string>("/email", "john.doe@example.com");

// Get values with type safety
auto name = doc.get<std::string>("/name");  // optional<string>
auto age = doc.get<int>("/age");            // optional<int>

// Work with nested objects
doc.set<std::string>("/address/city", "New York");
doc.set<std::string>("/address/zip", "10001");

// Arrays
doc.set<std::string>("/hobbies/0", "reading");
doc.set<std::string>("/hobbies/1", "gaming");
doc.set<std::string>("/hobbies/2", "coding");

// Serialize to JSON string
std::string json = doc.toString(2); // Pretty-print with 2-space indent
```

### Serializer - Automatic Type Mapping

```cpp
#include <nfx/serialization/json/Serializer.h>
#include <vector>
#include <map>

using namespace nfx::serialization::json;

// Simple example with STL containers
std::vector<int> numbers = {1, 2, 3, 4, 5};
std::string json = Serializer<std::vector<int>>::toString(numbers);
// json = "[1,2,3,4,5]"

std::vector<int> restored = Serializer<std::vector<int>>::fromString(json);
// restored == numbers

// Map example
std::map<std::string, int> scores = { {"Alice", 95}, {"Bob", 87} };
std::string scoresJson = Serializer<std::map<std::string, int>>::toString(scores);
// scoresJson = {"Alice":95,"Bob":87}

// With options
Serializer<std::vector<int>>::Options opts;
opts.prettyPrint = true;
std::string prettyJson = Serializer<std::vector<int>>::toString(numbers, opts);
```

### Array Iteration with Range-For

```cpp
#include <nfx/serialization/json/Document.h>

using namespace nfx::serialization::json;

auto docOpt = Document::fromString(R"({
    "items": [
        {"id": 1, "name": "Item A"},
        {"id": 2, "name": "Item B"},
        {"id": 3, "name": "Item C"}
    ]
})");

if (!docOpt)
{
    return 1;
}

// Get array and iterate with range-for
auto itemsOpt = docOpt->get<Document::Array>("items");
if (itemsOpt)
{
    for (const auto& item : itemsOpt.value())
    {
        auto id = item.get<int64_t>("id");
        auto name = item.get<std::string>("name");
        if (id && name)
        {
            std::cout << "ID: " << *id << ", Name: " << *name << std::endl;
        }
    }
}
```

### Object Iteration with Range-For

```cpp
#include <nfx/serialization/json/Document.h>

using namespace nfx::serialization::json;

auto docOpt = Document::fromString(R"({
    "config": {
        "timeout": 30,
        "retries": 3,
        "debug": true
    }
})");

if (!docOpt)
{
    return 1;
}

// Get object and iterate with range-for (structured bindings)
auto configOpt = docOpt->get<Document::Object>("config");
if (configOpt)
{
    for (const auto& [key, value] : configOpt.value())
    {
        std::cout << "Key: " << key << ", Value: " << value.toString() << std::endl;
    }
}
```

### SchemaValidator - JSON Schema Validation

```cpp
#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/SchemaValidator.h>

using namespace nfx::serialization::json;

// Define JSON Schema
auto schemaOpt = Document::fromString(R"({
    "type": "object",
    "required": ["name", "age"],
    "properties": {
        "name": {"type": "string"},
        "age": {"type": "integer", "minimum": 0, "maximum": 150},
        "email": {"type": "string", "format": "email"}
    }
})");

if (!schemaOpt)
{
    std::cerr << "Invalid schema JSON" << std::endl;
    return 1;
}

// Create validator
SchemaValidator validator(*schemaOpt);

// Validate document
auto dataOpt = Document::fromString(R"({"name": "John", "age": 30, "email": "john@example.com"})");
if (!dataOpt) {
    std::cerr << "Invalid data JSON" << std::endl;
    return 1;
}

ValidationResult result = validator.validate(*dataOpt);
if (result.isValid())
{
    std::cout << "Document is valid!" << std::endl;
}
else
{
    std::cout << "Validation errors:" << std::endl;
    for (const auto& error : result.errors())
    {
        std::cout << "  - " << error.toString() << std::endl;
    }
}
```

### SchemaGenerator - Automatic Schema Generation

```cpp
#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/SchemaGenerator.h>

using namespace nfx::serialization::json;

// Sample data
auto dataOpt = Document::fromString(R"({
    "name": "John Doe",
    "age": 30,
    "email": "john@example.com",
    "tags": ["developer", "manager"],
    "address": {
        "city": "New York",
        "zip": "10001"
    }
})");

if (!dataOpt)
{
    std::cerr << "Invalid data JSON" << std::endl;
    return 1;
}

// Generate schema with options
SchemaGenerator::Options opts;
opts.inferFormats = true;        // Detect email, date, URI, etc.
opts.title = "User Schema";
opts.description = "Generated from sample data";

SchemaGenerator generator(*dataOpt, opts);
const Document& schema = generator.schema();

// Output generated schema
std::cout << schema.toString(2) << std::endl;
```

**Sample Output:**

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "User Schema",
  "description": "Generated from sample data",
  "type": "object",
  "properties": {
    "name": {
      "type": "string"
    },
    "age": {
      "type": "integer"
    },
    "email": {
      "type": "string",
      "format": "email"
    },
    "tags": {
      "type": "array",
      "items": {
        "type": "string"
      }
    },
    "address": {
      "type": "object",
      "properties": {
        "city": {
          "type": "string"
        },
        "zip": {
          "type": "string"
        }
      },
      "required": ["city", "zip"]
    }
  },
  "required": ["name", "age", "email", "tags", "address"]
}
```

### Complete Example

```cpp
#include <iostream>
#include <vector>
#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/Serializer.h>

int main()
{
    using namespace nfx::serialization::json;

    // Create a JSON document
    Document doc;
    doc.set<std::string>( "/title", "My Application" );
    doc.set<std::string>( "/version", "1.0.0" );

    // Add array of users
    std::vector<std::string> users = { "Alice", "Bob", "Charlie" };
    std::string usersJson = Serializer<std::vector<std::string>>::toString( users );
    auto usersDoc = Document::fromString( usersJson );
    if ( usersDoc )
    {
        doc.set<Document>( "/users", *usersDoc );
    }

    // Add configuration object
    doc.set<bool>( "/config/debug", true );
    doc.set<int>( "/config/timeout", 30 );

    // Print JSON
    std::string json = doc.toString( 2 );
    std::cout << "Generated JSON:\n"
              << json << std::endl;

    // Iterate users with range-for
    auto usersArrayOpt = doc.get<Document::Array>( "/users" );
    if ( usersArrayOpt )
    {
        std::cout << "\nUsers:" << std::endl;
        for ( const auto& userDoc : usersArrayOpt.value() )
        {
            auto user = userDoc.get<std::string>( "" );
            if ( user )
            {
                std::cout << "  - " << *user << std::endl;
            }
        }
    }

    // Deserialize users back
    auto usersDocOpt = doc.get<Document>( "/users" );
    if ( usersDocOpt )
    {
        std::string usersArrayJson = usersDocOpt->toString();
        std::vector<std::string> restored = Serializer<std::vector<std::string>>::fromString( usersArrayJson );
        std::cout << "\nRestored " << restored.size() << " users" << std::endl;
    }

    return 0;
}
```

**Sample Output:**

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

The serializer can work with your custom container types and user-defined structures through a simple trait system. There are two main approaches:

#### Approach 1: Mark Custom Containers (Trait Specialization)

If you have a custom container that implements a standard container interface (iterators, typedefs, `insert`/`operator[]`), you can mark it as a container so the Serializer treats it like STL containers:

```cpp
// Your custom containers
template<typename K, typename V>
struct CustomHashMap {
    using key_type = K;
    using mapped_type = V;
    using value_type = std::pair<const K, V>;

    V& operator[](const K& key);
    auto begin() -> /* iterator */;
    auto end() -> /* iterator */;
    // ... other container methods
};

template<typename T>
struct CustomSet {
    using value_type = T;

    void insert(const T& value);
    auto begin() -> /* iterator */;
    auto end() -> /* iterator */;
    // ... other container methods
};

// Specialize the trait in the detail namespace
namespace nfx::serialization::json::detail {
    template<typename K, typename V>
    struct is_container<CustomHashMap<K,V>> : std::true_type {};

    template<typename T>
    struct is_container<CustomSet<T>> : std::true_type {};
}

// Now use them like STL containers
CustomHashMap<std::string, int> myMap;
myMap["answer"] = 42;
std::string json = Serializer<CustomHashMap<std::string, int>>::toString(myMap);
```

**Container Interface Requirements:**

For **sequence containers** (`vector`, `list`, `set`):

- `using value_type = T;`
- `begin()` / `end()` returning iterators
- `insert(const value_type&)` for deserialization

For **associative containers** (`map`, `unordered_map`):

- `using key_type = K;`
- `using mapped_type = V;`
- `using value_type = std::pair<const K, V>;`
- `operator[](const key_type&)` for deserialization
- `begin()` / `end()` returning iterators

#### Approach 2: Custom Serialization Logic (`SerializationTraits`)

For types that need special handling or don't fit the container model, implement `SerializationTraits`:

```cpp
struct Point3D {
    double x, y, z;
};

template<>
struct SerializationTraits<Point3D> {
    static void serialize(const Point3D& point, Document& doc) {
        doc.set<double>("/x", point.x);
        doc.set<double>("/y", point.y);
        doc.set<double>("/z", point.z);
    }

    static void deserialize(Point3D& point, const Document& doc) {
        point.x = doc.get<double>("/x").value_or(0.0);
        point.y = doc.get<double>("/y").value_or(0.0);
        point.z = doc.get<double>("/z").value_or(0.0);
    }
};

// Usage
Point3D origin{0, 0, 0};
std::string json = Serializer<Point3D>::toString(origin);
// Result: {"x":0.0,"y":0.0,"z":0.0}
```

**Key Points:**

- Container trait specializations go in `nfx::serialization::json::detail` namespace
- `SerializationTraits` specializations go in `nfx::serialization::json` namespace
- You don't modify library code - just expose the minimal interface
- Mix both approaches: use trait marking for containers, `SerializationTraits` for custom logic
- See `samples/Sample_JsonSerializer.cpp` for complete working examples

## Optional Extensions

nfx-serialization provides optional integration headers for other nfx libraries. These headers use conditional compilation (`__has_include()`) and are safe to include even if the external library is not installed:

### Available Extensions

- **`extensions/ContainersTraits.h`** - Serialization support for nfx-containers types
  - `nfx::containers::PerfectHashMap` - Compile-time perfect hash map
  - `nfx::containers::FastHashMap` - Runtime open-addressing hash map
  - `nfx::containers::FastHashSet` - Runtime open-addressing hash set
- **`extensions/DatatypesTraits.h`** - Serialization support for nfx-datatypes types
  - `nfx::datatypes::Int128` - 128-bit integer (cross-platform)
  - `nfx::datatypes::Decimal` - Fixed-point decimal arithmetic
- **`extensions/DateTimeTraits.h`** - Serialization support for nfx-datetime types
  - `nfx::time::DateTime` - Date and time representation (ISO 8601)
  - `nfx::time::DateTimeOffset` - Date and time with timezone offset
  - `nfx::time::TimeSpan` - Duration/time interval

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
         -DNFX_SERIALIZATION_BUILD_STATIC=ON \
         -DNFX_SERIALIZATION_BUILD_SHARED=ON \
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
├── benchmark/             # Performance benchmarks with Google Benchmark
├── cmake/                 # CMake modules and configuration
├── include/nfx/           # Public headers: Document, Serializer, SchemaValidator
├── samples/               # Example usage and demonstrations
├── src/                   # Implementation files
└── test/                  # Comprehensive unit tests with GoogleTest
```

## Performance

For detailed performance metrics and benchmarks, see the [benchmark documentation](benchmark/README.md).

## Roadmap

See [TODO.md](TODO.md) for upcoming features and project roadmap.

## Changelog

See the [CHANGELOG.md](CHANGELOG.md) for a detailed history of changes, new features, and bug fixes.

## License

This project is licensed under the MIT License.

## Dependencies

- **[nfx-stringutils](https://github.com/nfx-libs/nfx-stringutils)**: String validation utilities (MIT License) - Core dependency
- **[nlohmann/json](https://github.com/nlohmann/json)**: Modern JSON library for C++ (MIT License) - Core dependency

### Development Dependencies

- **[GoogleTest](https://github.com/google/googletest)**: Testing framework (BSD 3-Clause License) - Development only
- **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework (Apache 2.0 License) - Development only

All dependencies are automatically fetched via CMake FetchContent when building the library, tests, or benchmarks.

---

_Updated on December 20, 2025_
