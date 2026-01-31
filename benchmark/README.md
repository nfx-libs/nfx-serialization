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

| Platform    | OS         | Benchmark Framework     | C++ Compiler           | nfx-serialization Version | nfx-json Version |
| ----------- | ---------- | ----------------------- | ---------------------- | :-----------------------: | :--------------: |
| **Linux**   | LMDE 7     | Google Benchmark v1.9.4 | Clang 19.1.7-x64       |          v0.6.1           |      v1.1.0      |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MinGW GCC 14.2.0-x64   |          v0.6.1           |      v1.1.0      |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MSVC 19.44.35217.0-x64 |          v0.6.1           |      v1.1.0      |

---

# Performance Results

## JSON Serialization

### Document Serialization

| Operation                  | Linux Clang | Windows MinGW GCC | Windows MSVC |
| -------------------------- | ----------: | ----------------: | -----------: |
| **Serialize Small Object** |     96.6 ns |           __TBA__ |      __TBA__ |
| **Serialize Large Object** |      672 ns |           __TBA__ |      __TBA__ |
| **Serialize Pretty Print** |      761 ns |           __TBA__ |      __TBA__ |

### STL Containers: Builder vs Document

| Operation                   | Linux Clang | Windows MinGW GCC | Windows MSVC |
| --------------------------- | ----------: | ----------------: | -----------: |
| **IntVector Small Builder** |     97.3 ns |           __TBA__ |      __TBA__ |
| **IntVector Large Builder** |   82,561 ns |           __TBA__ |      __TBA__ |
| **StringIntMap Builder**    |    3,466 ns |           __TBA__ |      __TBA__ |

### Custom Types: Builder vs Document

| Operation                | Linux Clang | Windows MinGW GCC | Windows MSVC |
| ------------------------ | ----------: | ----------------: | -----------: |
| **Person Builder**       |      521 ns |           __TBA__ |      __TBA__ |
| **PersonVector Builder** |   50,292 ns |           __TBA__ |      __TBA__ |
| **Company Builder**      |   12,309 ns |           __TBA__ |      __TBA__ |

## Extension Types

### Absolute Performance

| Operation                | Linux Clang | Windows MinGW GCC | Windows MSVC |
| ------------------------ | ----------: | ----------------: | -----------: |
| **Int128 Single**        |      162 ns |           __TBA__ |      __TBA__ |
| **Int128 VectorSmall**   |    1.477 ns |           __TBA__ |      __TBA__ |
| **Int128 VectorLarge**   |  134,109 ns |           __TBA__ |      __TBA__ |
| **Decimal Single**       |     47.8 ns |           __TBA__ |      __TBA__ |
| **Decimal VectorSmall**  |      448 ns |           __TBA__ |      __TBA__ |
| **TimeSpan Single**      |     16.7 ns |           __TBA__ |      __TBA__ |
| **TimeSpan VectorSmall** |      175 ns |           __TBA__ |      __TBA__ |
| **DateTime Single**      |      412 ns |           __TBA__ |      __TBA__ |
| **DateTime VectorSmall** |    4,198 ns |           __TBA__ |      __TBA__ |

### Builder vs Document Comparison

| Operation                   | Linux Clang | Windows MinGW GCC | Windows MSVC |
| --------------------------- | ----------: | ----------------: | -----------: |
| **Int128 BuilderTraits**    |      160 ns |           __TBA__ |      __TBA__ |
| **Int128 DocumentTraits**   |      162 ns |           __TBA__ |      __TBA__ |
| **Decimal BuilderTraits**   |     43.1 ns |           __TBA__ |      __TBA__ |
| **Decimal DocumentTraits**  |     48.3 ns |           __TBA__ |      __TBA__ |
| **DateTime BuilderTraits**  |      406 ns |           __TBA__ |      __TBA__ |
| **DateTime DocumentTraits** |      410 ns |           __TBA__ |      __TBA__ |

---

_Updated on January 31, 2026_
