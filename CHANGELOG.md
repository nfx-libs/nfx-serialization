# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- NIL

### Changed

- NIL

### Deprecated

- NIL

### Removed

- NIL

### Fixed

- NIL

### Security

- NIL

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
