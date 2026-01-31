# Benchmarks

---

## Test Environment

### Hardware Configuration

| Component                | Specification                                                 |
| ------------------------ | ------------------------------------------------------------- |
| **Computer Model**       | Lenovo ThinkPad P15v Gen 3                                    |
| **CPU**                  | 12th Gen Intel Core i7-12800H (20 logical, 14 physical cores) |
| **Base Clock**           | 2.80 GHz                                                      |
| **Turbo Clock**          | 4.80 GHz                                                      |
| **L1 Data Cache**        | 48 KiB (×6 P-cores) + 32 KiB (×8 E-cores)                     |
| **L1 Instruction Cache** | 32 KiB (×6 P-cores) + 64 KiB (×2 E-core clusters)             |
| **L2 Unified Cache**     | 1.25 MiB (×6 P-cores) + 2 MiB (×2 E-core clusters)            |
| **L3 Unified Cache**     | 24 MiB (×1 shared)                                            |
| **RAM**                  | DDR4-3200 (32GB)                                              |
| **GPU**                  | NVIDIA RTX A2000 4GB GDDR6                                    |

### Software Configuration

| Platform    | OS         | Benchmark Framework     | C++ Compiler           | nfx-serialization Version |
| ----------- | ---------- | ----------------------- | ---------------------- | ------------------------- |
| **Linux**   | LMDE 7     | Google Benchmark v1.9.4 | Clang 19.1.7-x64       | v0.2.0                    |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MinGW GCC 14.2.0-x64   | v0.2.0                    |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MSVC 19.44.35217.0-x64 | v0.2.0                    |

---

# Performance Results

## JSON Serialization

### Document Serialization

| Operation                  | Linux Clang | Windows MinGW GCC | Windows MSVC |
| -------------------------- | ----------: | ----------------: | -----------: |
| **Serialize Small Object** |      104 ns |           __TBA__ |      __TBA__ |
| **Serialize Large Object** |      672 ns |           __TBA__ |      __TBA__ |
| **Serialize Pretty Print** |      766 ns |           __TBA__ |      __TBA__ |

### STL Containers: Builder vs Document

| Operation                    |  Linux Clang | Windows MinGW GCC | Windows MSVC |
| ---------------------------- | -----------: | ----------------: | -----------: |
| **IntVector Small Builder**  |      98.3 ns |           __TBA__ |      __TBA__ |
| **IntVector Small Document** |     1,166 ns |           __TBA__ |      __TBA__ |
| **IntVector Large Builder**  |    82,561 ns |           __TBA__ |      __TBA__ |
| **IntVector Large Document** | 1,346,454 ns |           __TBA__ |      __TBA__ |
| **StringIntMap Builder**     |     3,466 ns |           __TBA__ |      __TBA__ |
| **StringIntMap Document**    |    51,369 ns |           __TBA__ |      __TBA__ |

### Custom Types: Builder vs Document

| Operation                 | Linux Clang | Windows MinGW GCC | Windows MSVC |
| ------------------------- | ----------: | ----------------: | -----------: |
| **Person Builder**        |      536 ns |           __TBA__ |      __TBA__ |
| **Person Document**       |      505 ns |           __TBA__ |      __TBA__ |
| **PersonVector Builder**  |   50,636 ns |           __TBA__ |      __TBA__ |
| **PersonVector Document** |   62,696 ns |           __TBA__ |      __TBA__ |
| **Company Builder**       |    8,671 ns |           __TBA__ |      __TBA__ |
| **Company Document**      |    8,511 ns |           __TBA__ |      __TBA__ |

## Extension Types

### Absolute Performance

| Operation                | Linux Clang | Windows MinGW GCC | Windows MSVC |
| ------------------------ | ----------: | ----------------: | -----------: |
| **Int128 Single**        |      209 ns |           __TBA__ |      __TBA__ |
| **Int128 VectorSmall**   |    2,853 ns |           __TBA__ |      __TBA__ |
| **Int128 VectorLarge**   |  267,113 ns |           __TBA__ |      __TBA__ |
| **Decimal Single**       |     91.2 ns |           __TBA__ |      __TBA__ |
| **Decimal VectorSmall**  |    1,797 ns |           __TBA__ |      __TBA__ |
| **TimeSpan Single**      |     50.3 ns |           __TBA__ |      __TBA__ |
| **TimeSpan VectorSmall** |    1,225 ns |           __TBA__ |      __TBA__ |
| **DateTime Single**      |      481 ns |           __TBA__ |      __TBA__ |
| **DateTime VectorSmall** |    6,261 ns |           __TBA__ |      __TBA__ |

### Builder vs Document Comparison

| Operation                        | Linux Clang | Windows MinGW GCC | Windows MSVC |
| -------------------------------- | ----------: | ----------------: | -----------: |
| **Int128 BuilderTraits**         |      163 ns |           __TBA__ |      __TBA__ |
| **Int128 SerializationTraits**   |      227 ns |           __TBA__ |      __TBA__ |
| **Decimal BuilderTraits**        |     43.1 ns |           __TBA__ |      __TBA__ |
| **Decimal SerializationTraits**  |     90.5 ns |           __TBA__ |      __TBA__ |
| **DateTime BuilderTraits**       |      414 ns |           __TBA__ |      __TBA__ |
| **DateTime SerializationTraits** |      465 ns |           __TBA__ |      __TBA__ |

---

_Updated on January 31, 2026_
