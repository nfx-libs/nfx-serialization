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
| **Linux**   | LMDE 7     | Google Benchmark v1.9.4 | Clang 19.1.7-x64       |          v0.7.0           |      v1.1.0      |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MinGW GCC 14.2.0-x64   |          v0.7.0           |      v1.1.0      |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MSVC 19.44.35217.0-x64 |          v0.7.0           |      v1.1.0      |

---

# Performance Results

## JSON Serialization

### Simple Object (3 fields)

| API          | Linux Clang | Windows MinGW GCC | Windows MSVC |
| ------------ | ----------: | ----------------: | -----------: |
| **Document** |      348 ns |            536 ns |       460 ns |
| **Builder**  |      114 ns |            141 ns |       131 ns |

### Int Array (10 elements)

| API            | Linux Clang | Windows MinGW GCC | Windows MSVC |
| -------------- | ----------: | ----------------: | -----------: |
| **Document**   |      264 ns |            446 ns |       691 ns |
| **Builder**    |     98.6 ns |            107 ns |       146 ns |
| **Serializer** |     94.7 ns |            114 ns |       175 ns |

### Int Array (10,000 elements)

| API            | Linux Clang | Windows MinGW GCC | Windows MSVC |
| -------------- | ----------: | ----------------: | -----------: |
| **Document**   |  347,804 ns |        478,896 ns |   773,882 ns |
| **Builder**    |   80,529 ns |        102,205 ns |   174,099 ns |
| **Serializer** |   82,174 ns |        102,554 ns |   154,258 ns |

### String-Int Map (100 pairs)

| API            | Linux Clang | Windows MinGW GCC | Windows MSVC |
| -------------- | ----------: | ----------------: | -----------: |
| **Document**   |   46,557 ns |         79,778 ns |    93,373 ns |
| **Builder**    |    2,474 ns |          3,184 ns |     3,123 ns |
| **Serializer** |    3,487 ns |          4,522 ns |     5,216 ns |

### Point2D (2 doubles)

| API          | Linux Clang | Windows MinGW GCC | Windows MSVC |
| ------------ | ----------: | ----------------: | -----------: |
| **Document** |      280 ns |            501 ns |       460 ns |
| **Builder**  |      138 ns |            289 ns |       163 ns |
| **Traits**   |      121 ns |            319 ns |       160 ns |

### Person (4 fields)

| API          | Linux Clang | Windows MinGW GCC | Windows MSVC |
| ------------ | ----------: | ----------------: | -----------: |
| **Document** |      456 ns |            660 ns |       784 ns |
| **Builder**  |      139 ns |            176 ns |       142 ns |
| **Traits**   |      159 ns |            268 ns |       235 ns |
| **Legacy**   |      512 ns |            729 ns |       828 ns |

### Person Vector (100 elements)

| API          | Linux Clang | Windows MinGW GCC | Windows MSVC |
| ------------ | ----------: | ----------------: | -----------: |
| **Document** |   63,447 ns |        107,574 ns |   124,500 ns |
| **Builder**  |   11,029 ns |         15,266 ns |    11,802 ns |
| **Traits**   |   11,426 ns |         16,157 ns |    13,819 ns |
| **Legacy**   |   48,929 ns |         73,755 ns |    84,124 ns |

### Company (nested with 10 staff)

| API          | Linux Clang | Windows MinGW GCC | Windows MSVC |
| ------------ | ----------: | ----------------: | -----------: |
| **Document** |    6,363 ns |         10,635 ns |    11,125 ns |
| **Builder**  |    1,254 ns |          1,697 ns |     1,364 ns |
| **Traits**   |    1,351 ns |          1,945 ns |     1,925 ns |
| **Legacy**   |    7,087 ns |          9,942 ns |    11,002 ns |

### Large Document (19 fields)

| API             | Linux Clang | Windows MinGW GCC | Windows MSVC |
| --------------- | ----------: | ----------------: | -----------: |
| **Document**    |    2,986 ns |          4,326 ns |     5,644 ns |
| **PrettyPrint** |    3,027 ns |          4,550 ns |      6,052ns |

---

_Updated on February 01, 2026_
