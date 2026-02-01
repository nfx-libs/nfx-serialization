# nfx-serialization Samples

This directory contains tutorial samples demonstrating the nfx-serialization library's capabilities. Each sample is self-contained and progressively introduces concepts from basic JSON operations to advanced custom type serialization.

## 📚 Sample Programs

### 1. Sample_JsonSerializationBasics.cpp

**Fundamental JSON operations with Document and Builder APIs**

Demonstrates the core nfx-json APIs that form the foundation of nfx-serialization:
- **Document API**: DOM-based JSON navigation with type-safe access
- **Builder API**: Streaming JSON generation (SAX-like, optimal performance)

**10 sections covering:**
1. **Document API**: Parsing and type-safe extraction (`get<T>()`, `std::optional`)
2. **Document API**: Path navigation with dot notation (`company.name`, `company.headquarters.city`)
3. **Document API**: JSON Pointer navigation (RFC 6901: `/users/0/name`)
4. **Document API**: Iterating over arrays and objects (range-based for loops)
5. **Builder API**: Creating JSON from primitives (int, double, string, bool, null)
6. **Builder API**: Creating JSON objects (`startObject()`, `key()`, `endObject()`)
7. **Builder API**: Creating JSON arrays (`startArray()`, `write()`, `endArray()`)
8. **Builder API**: Nested structures (objects within arrays, arrays within objects)
9. **Roundtrip validation**: Builder → Document → Builder (data integrity verification)
10. **Serializer API**: High-level convenience methods (`toString()`, `fromString()`)

**Key takeaways:**
- Document API provides type-safe random access (DOM-based)
- Builder API offers streaming generation with zero DOM overhead
- Both APIs are complementary and work seamlessly together
- Roundtrip validation ensures data fidelity

**Run:**
```bash
./build/bin/Sample_JsonSerializationBasics
```

---

### 2. Sample_JsonSerializationContainers.cpp

**Automatic STL container serialization without custom code**

Demonstrates how nfx-serialization automatically handles standard C++ containers through compile-time type detection. No manual serialization code needed!

**9 sections covering:**
1. **Sequential containers**: `vector`, `list`, `set` → JSON arrays
2. **Associative containers**: `map`, `unordered_map` → JSON objects
3. **std::optional**: Null handling with `nullopt` → JSON `null`
4. **Smart pointers**: `unique_ptr`, `shared_ptr` serialization (transparent handling)
5. **Nested containers**: `vector<vector<int>>` → 2D JSON arrays
6. **Complex nesting**: `map<string, vector<string>>` (departments with employees)
7. **Real-world example**: Application metrics with statistics computation
8. **JSON → STL**: Flexible deserialization from JSON strings
9. **Roundtrip testing**: Complex nested structures (map<string, map<string, vector<int>>>)

**Key features demonstrated:**
- Zero custom code for STL types
- Automatic type detection at compile-time
- Nested containers work seamlessly
- `std::map` preserves alphabetical key order (deterministic output)
- `std::unordered_map` has non-deterministic order (hash-based)
- Roundtrip validation for data integrity

**Run:**
```bash
./build/bin/Sample_JsonSerializationContainers
```

---

### 3. Sample_JsonSerializationTraits.cpp

**Custom type serialization with SerializationTraits**

Demonstrates the extensibility of nfx-serialization through the `SerializationTraits` trait system. Shows how to implement custom serialization for user-defined types using the asymmetric read/write architecture.

**3 sections covering:**
1. **Simple POD struct**: `Point2D` with x, y coordinates
   - Basic `serialize()` with Builder (streaming write)
   - Basic `fromDocument()` (DOM-based read)
   - Roundtrip validation

2. **SerializationTraits with STL containers**: `Person` with name, age, hobbies
   - Combining custom types with STL containers
   - Array serialization within custom traits
   - Type-safe deserialization with Document API

3. **Nested custom types**: `Company` with `Address` and employees
   - Composition of multiple SerializationTraits
   - Nested objects and arrays
   - Complex real-world data structures
   - Demonstrates scalability of the trait system

**Architecture highlights:**
- **Asymmetric read/write pattern**:
  - **Write**: `serialize(const T&, Builder&)` - Streaming JSON generation (zero DOM overhead)
  - **Read**: `fromDocument(const Document&, T&)` - Type-safe DOM navigation
- **Trait composition**: Custom types can contain other custom types
- **STL integration**: Custom types work seamlessly with STL containers
- **Compile-time dispatch**: SFINAE-based automatic trait detection

**Run:**
```bash
./build/bin/Sample_JsonSerializationTraits
```

---

## 🏗️ Building Samples

Samples are built automatically when `NFX_SERIALIZATION_BUILD_SAMPLES` is enabled:

```bash
# Configure with samples enabled
cmake -B build -DCMAKE_BUILD_TYPE=Release -DNFX_SERIALIZATION_BUILD_SAMPLES=ON

# Build all samples
cmake --build build --config Release

# Run individual samples
./build/bin/Sample_JsonSerializationBasics
./build/bin/Sample_JsonSerializationContainers
./build/bin/Sample_JsonSerializationTraits
```
---

_Updated on February 01, 2026_
