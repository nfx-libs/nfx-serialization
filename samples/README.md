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

**4 sections covering:**
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

4. **Factory deserialization**: `ImmutablePerson` with deleted default constructor
   - Static factory method `fromDocument(const Document&)` returns new instance
   - SFINAE-based automatic detection via `has_factory_deserialization_v<T>`
   - Enables serialization of immutable types (const members, no default constructor)
   - Nested factory deserialization (immutable types containing other immutable types)
   - Automatic dispatch: factory pattern when available, mutable pattern otherwise

**Architecture highlights:**
- **Asymmetric read/write pattern**:
  - **Write**: `serialize(const T&, Builder&)` - Streaming JSON generation (zero DOM overhead)
  - **Read**: `fromDocument(const Document&, T&)` - Type-safe DOM navigation (mutable pattern)
  - **Read (alternative)**: `static T fromDocument(const Document&)` - Factory pattern (immutable types)
- **Trait composition**: Custom types can contain other custom types
- **STL integration**: Custom types work seamlessly with STL containers
- **Compile-time dispatch**: SFINAE-based automatic trait detection
- **Immutable type support**: Factory deserialization for types with deleted default constructors

**Run:**
```bash
./build/bin/Sample_JsonSerializationTraits
```

---

### 4. Sample_JsonSerializationStlContainers.cpp

**Guide to all STL container types**

Complete reference covering every standard C++ container supported by nfx-serialization. Progressive tutorial from simple types to complex associative containers.

**20 sections covering:**
1. **std::pair**: Tuple of 2 elements → `[first, second]`
2. **std::tuple**: Heterogeneous tuple → `[elem0, elem1, ...]`
3. **std::optional**: Nullable values → value or `null`
4. **std::variant**: Tagged union → `{"tag": "TypeName", "data": value}`
5. **std::array**: Fixed-size array → JSON array
6. **std::vector**: Dynamic array → JSON array
7. **std::deque**: Double-ended queue → JSON array
8. **std::list**: Doubly-linked list → JSON array
9. **std::forward_list**: Singly-linked list → JSON array (forward-only iteration)
10. **std::set**: Ordered unique set → JSON array (sorted)
11. **std::multiset**: Ordered set with duplicates → JSON array (sorted, duplicates preserved)
12. **std::unordered_set**: Hash-based unique set → JSON array
13. **std::unordered_multiset**: Hash-based set with duplicates → JSON array
14. **std::map**: Ordered key-value map → JSON object `{"key": value}`
15. **std::multimap**: Ordered map with duplicate keys → `[{"key": K, "value": V}, ...]`
16. **std::unordered_map**: Hash-based map → JSON object
17. **std::unordered_multimap**: Hash-based map with duplicate keys → `[{"key": K, "value": V}, ...]`
18. **std::unique_ptr**: Unique smart pointer → value or `null`
19. **std::shared_ptr**: Shared smart pointer → value or `null`
20. **std::span**: Non-owning view (C++20) → JSON array (serialization only)

**Key features demonstrated:**
- Complete STL container reference in one place
- JSON format for each container type clearly documented
- Roundtrip validation for each type
- Special handling for duplicate keys/values (multimap, multiset)
- Cross-language compatibility notes (variant, multimap format)
- std::forward_list uses push_front with reversal for deserialization
- std::span serialization-only semantics (cannot deserialize to view)
- Performance characteristics noted where relevant

**Run:**
```bash
./build/bin/Sample_JsonSerializationStlContainers
```

---

### 5. Sample_JsonSerializationNfxTypes.cpp

**Guide to nfx library extension types**

Complete reference covering all nfx library types with compile-time detection. Progressive tutorial demonstrating high-performance containers, arbitrary-precision numerics, and ISO 8601 datetime serialization.

**11 sections covering:**
1. **nfx::containers::PerfectHashMap**: Immutable perfect hash map → `[{"key": K, "value": V}, ...]` (CHD algorithm, O(1) guaranteed)
2. **nfx::containers::FastHashMap**: High-performance hash map → `[{"key": K, "value": V}, ...]` (inline storage)
3. **nfx::containers::FastHashSet**: High-performance hash set → JSON array
4. **nfx::containers::OrderedHashMap**: Insertion-order preserving map → `[{"key": K, "value": V}, ...]`
5. **nfx::containers::OrderedHashSet**: Insertion-order preserving set → JSON array
6. **nfx::containers::SmallVector**: Stack-optimized vector → JSON array (avoids heap for small sizes)
7. **nfx::datatypes::Int128**: 128-bit integer → JSON string (preserves precision beyond JSON number limits)
8. **nfx::datatypes::Decimal**: Arbitrary-precision decimal → JSON string (financial calculations)
9. **nfx::datetime::DateTime**: Date and time → ISO 8601 string `"YYYY-MM-DDTHH:MM:SS.sssZ"`
10. **nfx::datetime::DateTimeOffset**: Date, time, timezone → ISO 8601 with offset `"YYYY-MM-DDTHH:MM:SS.sss+HH:MM"`
11. **nfx::datetime::TimeSpan**: Duration/interval → int64 (ticks)

**Key features demonstrated:**
- Compile-time detection using `__has_include()` for optional nfx dependencies
- Perfect hashing with no collisions (PerfectHashMap: `count()` vs `size()`)
- High-performance containers optimized for specific use cases
- Arbitrary-precision arithmetic avoiding floating-point errors
- ISO 8601 datetime serialization for cross-platform compatibility
- Adaptive sample execution based on available libraries
- Heterogeneous lookup support in hash containers

**Run:**
```bash
./build/bin/Sample_JsonSerializationNfxTypes
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
./build/bin/Sample_JsonSerializationStlContainers
./build/bin/Sample_JsonSerializationNfxTypes
```
---

_Updated on February 01, 2026_
