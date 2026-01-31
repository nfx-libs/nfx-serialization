# Changelog

## [Unreleased]

### Added

- **Serializer**: Added streaming serialization with BuilderTraits API
  - New `BuilderTraits<T>` template for streaming JSON generation via Builder API
  - Eliminates Document intermediate representation for performance improvement
  - BuilderTraits specializations for all nfx extension types (containers, datatypes, datetime)
  - New sample: `Sample_JsonSerializerWithBuilder.cpp`
  - New tests: `Tests_JsonSerializerBuilder.cpp`
- **Serializer**: Added native `std::pair<TFirst, TSecond>` support in core serializer
  - Moved from ContainersTraits extension to built-in Serializer functionality
  - Serializes as JSON object with `"first"` and `"second"` fields
  - Supports nested deserialization with proper error handling

### Changed

- **Breaking**: Removed `toDocument()` and `fromDocument()` from public Serializer API
  - Serializer now uses **only** `toString()` and `fromString()` static methods for serialization
  - Removed instance methods `toDocument(const T&)` and `fromDocument(const Document&)`
  - Internal serialization now streams directly to Builder (no intermediate Document)
  - DocumentTraits still used internally for deserialization from Document
  - User code must use `Serializer<T>::toString(obj)` and `Serializer<T>::fromString(json)` only
- **Breaking**: Renamed `SerializationTraits` to `DocumentTraits` with updated method signatures
  - File renamed from `SerializationTraits.h` to `DocumentTraits.h` (in `traits/` subfolder)
  - Methods renamed: `serialize()` → `toDocument()`, `deserialize()` → `fromDocument()`
  - Method signature change: `fromDocument(const Document& doc, T& obj)` - document parameter now comes first
  - Affects all custom types and extension traits (ContainersTraits, DatatypesTraits, DateTimeTraits)
  - User code with custom serialization must update method names and signatures
  - **Note**: DocumentTraits now primarily used for deserialization; serialization uses BuilderTraits for performance
- **Breaking**: Simplified `toDocument()` custom method signature - single variant only
  - User types must now implement: `void toDocument(const Serializer<T>&, Document&) const`
  - Removed support for `Document toDocument()` (no serializer access)
  - Removed support for `Document toDocument(const Serializer<T>&)` (functional style)
  - Single signature provides serializer options access and document reuse for performance
  - All samples and tests updated to use new signature
- **Architecture**: Simplified container serialization by removing nfx-specific container handling from Serializer
  - Removed specialized nfx container logic (PerfectHashMap, FastHashMap, FastHashSet handling) from core Serializer
  - nfx containers now exclusively handled through DocumentTraits specializations in ContainersTraits.h extension
  - Cleaner separation: Serializer handles STL containers, extensions handle nfx containers
- Updated nfx-json dependency from 1.0.3 to 1.1.0

### Deprecated

- NIL

### Removed

- **Breaking**: Removed `SerializableDocument` class completely
  - SerializableDocument was an intermediate wrapper around Document used by old serialization API
  - No longer needed - serialization now streams directly to Builder for performance
  - Deserialization works directly with nfx::json::Document
  - All functionality replaced by `Serializer<T>::toString()` and `Serializer<T>::fromString()`
- **Breaking**: Removed `toDocument()` and `fromDocument()` instance methods from Serializer
  - These methods returned/accepted SerializableDocument
  - Use static methods `toString()` and `fromString()` instead
- **Breaking**: Removed `std::pair` support from ContainersTraits extension
  - `std::pair` is now a built-in type in core Serializer (see Added section)
  - No longer requires including ContainersTraits.h for std::pair serialization

### Fixed

- **Serializer**: Fixed unreachable dead code in `serializeValue()` fallback path
  - Removed duplicate `BuilderTraits` check that was impossible to reach
  - Simplified final fallback - now compilation fails if type has no BuilderTraits or built-in support
- **Serializer**: Optimized type resolution order in `serializeValue()` for better performance
  - Reordered checks: BuilderTraits → Built-in types → Custom toDocument() method
  - Built-in types (bool, int, string, containers) now bypass Document intermediate representation
  - Custom `toDocument()` methods still supported but with documented performance implications

### Security

- NIL

## [0.6.1] - 2026-01-27

### Changed

- Updated nfx-json dependency from 1.0.2 to 1.0.3
- Updated nfx-containers dependency from 0.2.0 to 0.3.0 (extension tests)
- Updated Google Benchmark dependency from 1.9.4 to 1.9.5

## [0.6.0] - 2026-01-27

### Changed

- **Architecture**: Transitioned to focused C++ serialization library built on nfx-json
  - nfx-serialization is now header-only (INTERFACE library)
  - Core dependency: nfx-json 1.0.0 (provides Document, SchemaValidator, SchemaGenerator)
  - Removed dependency on nfx-stringutils
  - CMake target: `nfx-serialization::nfx-serialization` (header-only interface)

### Removed

- **Breaking**: Extracted JSON core functionality to separate `nfx-json` library
  - Removed `Document`, `Object`, `Array`, `PathView` classes (now in nfx-json)
  - Removed `SchemaValidator` and `SchemaGenerator` classes (now in nfx-json)
  - Removed JSON parsing, manipulation, and validation APIs
  - nfx-serialization now focuses solely on C++ type ↔ JSON serialization
  - nfx-json 1.0.0 is now a required dependency
  - Users must now include `<nfx/json/Document.h>` instead of `<nfx/serialization/json/Document.h>`
  - Namespace changed: `nfx::serialization::json::Document` → `nfx::json::Document`

## [0.5.0] - 2026-01-12

### Added

- **Document API**: Added `set<T>(path, const char*)` and `set<T, U>(path, U)` overloads for types with explicit constructors
  - Enables direct setting of types like `Decimal` from string literals: `doc.set<Decimal>("path", "123.456")`
  - Enables direct setting from arithmetic types: `doc.set<Decimal>("path", 2.5)` or `doc.set<Decimal>("path", 42)`
  - Works with any type constructible from `const char*` or arithmetic types (int, float, double, etc.)
  - Uses C++20 constraints to avoid conflicts with existing JSON value types
- **ContainersTraits**: Added JSON serialization support for `nfx::containers::SmallVector`
- **ContainersTraits**: Added JSON serialization support for `std::pair<TFirst, TSecond>`
  - Serializes as JSON object with `"first"` and `"second"` fields
  - Enables serialization of containers like `std::vector<std::pair<K, V>>`

### Changed

- **Performance**: Document now uses `std::shared_ptr<Document_impl>` for reference-counted internal data
  - Document copy constructor and assignment now perform shallow copy (shared ownership) instead of deep copy
  - Significantly improves iterator performance by eliminating redundant deep copies when dereferencing `Object::Iterator` and `Array::Iterator`
  - Multiple Document instances can share the same underlying JSON data (similar to STL container semantics)
  - Note: Document follows STL container thread-safety model - concurrent modifications require external synchronization


## [0.4.0] - 2026-01-10

### Added

- **Document API**: Added direct `Document::set<T>()`, `Document::get<T>()`, and `Document::is<T>()` support for custom types
  - nfx extension types via `SerializationTraits`: `Decimal`, `Int128`, `DateTime`, `DateTimeOffset`, `TimeSpan`, `FastHashMap`, `FastHashSet`, `PerfectHashMap`
  - STL types via `Serializer<T>`: `std::vector`, `std::map`, `std::set`, `std::list`, `std::deque`, `std::unique_ptr`, `std::shared_ptr`, `std::optional`
  - Supports nested containers (e.g., `std::vector<std::vector<int>>`)
  - Type traits for template selection: `has_serialization_traits`, `is_nfx_extension_type`
- **Concepts**: Updated `JsonValue` concept to exclude types with `SerializationTraits` specializations for proper template overload resolution

### Changed

- **Breaking**: Updated `DateTimeTraits.h` to use `nfx-datetime` 0.2.0 consolidated formatting API
  - Replaced deprecated `toIso8601Extended()` with `toString(DateTime::Format::Iso8601Precise)`
  - DateTime serialization now outputs fractional seconds with exactly 7 digits (tick precision) instead of stripping trailing zeros
  - DateTimeOffset serialization now uses explicit offset format (±HH:MM) instead of 'Z' notation for UTC
- ### Fixed

- **Serializer**: Fixed silent type conversion during deserialization
  - Now throws exceptions when primitive type deserialization fails (e.g., int to string)
  - Prevents `std::vector<int>` from being incorrectly deserialized as `std::vector<std::string>` with empty strings
  - Ensures `is<T>()` correctly rejects incompatible types

## [0.3.0] - 2025-12-20

### Added

- **Tests**: Added tests for nested `Object` and `Array` access via `get<Object>()` and `get<Array>()` methods

### Removed

- **Breaking**: Removed `std::string_view` support from `Document`, `Object`, and `Array` classes
  - `get<std::string_view>()`, `set<std::string_view>()`, and `append<std::string_view>()` are no longer available
  - This API created dangling references by returning views to temporary strings
- **Breaking**: Removed `Serializer<T>` instance methods `serializeToString()` and `deserializeFromString()`
  - These methods were redundant - use static methods `Serializer<T>::toString()` and `Serializer<T>::fromString()` instead
  - Instance methods `serialize()` and `deserialize()` (working with `Document`) are still available

### Fixed

- **Critical**: Fixed dangling pointer bugs in `Object::get<Object>(path)` and `Object::get<Array>(path)` methods
  - These methods were creating temporary `Document` objects and returning `Object`/`Array` wrappers with pointers to the temporaries
  - After function return, the temporary was destroyed, leaving dangling pointers causing undefined behavior and segmentation faults
  - Now correctly returns `Object`/`Array` with references to the parent document and proper path
- **Critical**: Fixed dangling pointer bugs in `Array::get<Object>(path)` and `Array::get<Array>(path)` methods
  - Same issue as above - temporary documents were being destroyed after return
  - Now correctly returns `Object`/`Array` with references to the parent document and proper path

## [0.2.0] - 2025-11-30

### Added

- **nfx-stringutils dependency**: Added nfx-stringutils as a FetchContent dependency for string validation functions

### Changed

- **Format validation**: Replaced all `std::regex` usage with constexpr `nfx::string::` functions for ~50% performance improvement
- **SchemaGenerator**: Improved IDN/IRI detection for email, hostname, and URI format inference

### Removed

- **Regex.h**: Deleted `nfx::serialization::json::regex` namespace (replaced by nfx-stringutils)

## [0.1.0] - 2025-11-30 - Initial Release

### Added

- **Document**: JSON document abstraction with JSON Pointer path support

  - Type-safe value access (`get<T>`, `set<T>`, `has`, `remove`)
  - Nested object and array operations
  - Factory methods (`fromJsonString`, `fromFile`)
  - Iterator support for arrays and object fields (`Array`, `Object`, `PathView`)
  - Comparison operators and copy/move semantics

- **SchemaValidator**: JSON Schema Draft 2020-12 validation

  - Comprehensive type and constraint validation
  - Format validation (date-time, email, URI, UUID, IPv4/IPv6, etc.)
  - Schema composition (`allOf`, `anyOf`, `oneOf`, `not`, `if/then/else`)
  - `$ref` and `$anchor` resolution
  - Detailed error reporting with `ValidationResult` and `ValidationError`
  - Configurable via Options struct (`strictMode`, `maxDepth`)

- **SchemaGenerator**: JSON Schema Draft 2020-12 generation from samples

  - Single and multi-sample analysis
  - Format detection (email, date, URI, UUID, etc.)
  - Constraint inference (`minLength`, `minimum`, `maximum`, `pattern`)
  - Enum detection from limited value sets
  - Configurable via Options struct

- **Serializer**: Templated JSON serialization with automatic type mapping

  - Compile-time type detection and field mapping
  - Support for POD types, containers, and nested structures
  - `toJson`/`fromJson` static convenience methods
  - `serialize`/`deserialize` instance methods
  - Configurable via Options struct

- **Documentation**

  - README with feature overview
  - Detailed API documentation with Doxygen comments
  - Sample application demonstrating library usage
  - Build and installation instructions

- **Testing & Benchmarking**
  - Unit test suite
  - Performance benchmarks for all operations
  - Cross-compiler performance validation
