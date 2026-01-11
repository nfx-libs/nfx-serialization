# Changelog

## [Unreleased]

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

### Deprecated

- NIL

### Removed

- NIL

### Fixed

- NIL


### Security

- NIL

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
