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

| Platform    | OS         | Benchmark Framework     | C++ Compiler              | nfx-serialization Version |
| ----------- | ---------- | ----------------------- | ------------------------- | ------------------------- |
| **Linux**   | LMDE 7     | Google Benchmark v1.9.4 | GCC 14.2.0-x64            | v0.1.0                    |
| **Linux**   | LMDE 7     | Google Benchmark v1.9.4 | Clang 19.1.7-x64          | v0.1.0                    |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MinGW GCC 14.2.0-x64      | v0.1.0                    |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | Clang-GNU-CLI 19.1.5-x64  | v0.1.0                    |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | Clang-MSVC-CLI 19.1.5-x64 | v0.1.0                    |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MSVC 19.44.35217.0-x64    | v0.1.0                    |

---

# Performance Results

## JSON Access

### Value Retrieval

| Operation                   | Linux GCC  | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------- | ---------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Get by Key (Top Level)**  | 32.5 ns    | 34.1 ns     | 36.6 ns           | **30.3 ns**           | 35.4 ns                | 45.0 ns      |
| **Get by Key (Nested)**     | 67.3 ns    | **66.5 ns** | 80.9 ns           | 80.3 ns               | 76.7 ns                | 98.7 ns      |
| **Get by Pointer (Top)**    | 35.7 ns    | 35.6 ns     | 36.6 ns           | **29.0 ns**           | 35.0 ns                | 50.5 ns      |
| **Get by Pointer (Nested)** | 77.2 ns    | 74.1 ns     | 84.5 ns           | **69.6 ns**           | 81.6 ns                | 111 ns       |
| **Get Deep Nested (3 Lvl)** | 98.0 ns    | **97.4 ns** | 120 ns            | 99.6 ns               | 110 ns                 | 138 ns       |
| **Get Deep Nested (4 Lvl)** | **123 ns** | **123 ns**  | 153 ns            | 124 ns                | 136 ns                 | 173 ns       |
| **Has Field (Exists)**      | 28.4 ns    | 26.9 ns     | 30.8 ns           | **24.6 ns**           | 34.1 ns                | 36.5 ns      |
| **Has Field (Not Exists)**  | 27.5 ns    | 30.7 ns     | 26.9 ns           | **21.8 ns**           | 26.0 ns                | 35.2 ns      |
| **Has Field (Nested)**      | 145 ns     | 141 ns      | 162 ns            | **128 ns**            | 153 ns                 | 191 ns       |

## JSON Iteration

### Container Traversal

| Operation                   | Linux GCC    | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------- | ------------ | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Object Field Iteration**  | **781 ns**   | 940 ns      | 1,884 ns          | 1,672 ns              | 1,867 ns               | 1,367 ns     |
| **Array Element Iteration** | **7,970 ns** | 9,263 ns    | 20,223 ns         | 19,953 ns             | 21,218 ns              | 21,144 ns    |
| **Nested Object Iteration** | **3,362 ns** | 4,023 ns    | 9,893 ns          | 11,934 ns             | 9,998 ns               | 10,143 ns    |

## JSON Modification

### Document Mutation

| Operation                  | Linux GCC    | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| -------------------------- | ------------ | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Set Primitive (String)** | **107 ns**   | 116 ns      | 269 ns            | 245 ns                | 270 ns                 | 338 ns       |
| **Set Primitive (Int)**    | **95.6 ns**  | 107 ns      | 226 ns            | 220 ns                | 235 ns                 | 278 ns       |
| **Set Primitive (Bool)**   | **94.9 ns**  | 106 ns      | 238 ns            | 219 ns                | 240 ns                 | 285 ns       |
| **Set Primitive (Double)** | 95.5 ns      | **105 ns**  | 239 ns            | 222 ns                | 241 ns                 | 280 ns       |
| **Set Nested (2 Levels)**  | **168 ns**   | 183 ns      | 401 ns            | 359 ns                | 403 ns                 | 465 ns       |
| **Set Nested (3 Levels)**  | **233 ns**   | 249 ns      | 533 ns            | 479 ns                | 513 ns                 | 604 ns       |
| **Set Nested (4 Levels)**  | **293 ns**   | 332 ns      | 663 ns            | 598 ns                | 644 ns                 | 824 ns       |
| **Add Object Field (Seq)** | **660 ns**   | 705 ns      | 2,372 ns          | 2,478 ns              | 1,656 ns               | 3,221 ns     |
| **Append Array Element**   | **203 ns**   | 265 ns      | 465 ns            | 621 ns                | 654 ns                 | 740 ns       |
| **Append to Large Array**  | **1,349 ns** | 1,655 ns    | 1,921 ns          | 3,141 ns              | 3,355 ns               | 3,783 ns     |

## Object Serialization

### Serializer Performance

| Operation                     | Linux GCC    | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ----------------------------- | ------------ | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Serialize POD**             | **281 ns**   | 330 ns      | 565 ns            | 560 ns                | 642 ns                 | 751 ns       |
| **Deserialize POD**           | **76.6 ns**  | 79.6 ns     | 79.7 ns           | 80.7 ns               | 89.9 ns                | 120 ns       |
| **Serialize Medium Object**   | **428 ns**   | 483 ns      | 868 ns            | 937 ns                | 1,043 ns               | 1,282 ns     |
| **Deserialize Medium Object** | **142 ns**   | **142 ns**  | 186 ns            | 156 ns                | 173 ns                 | 255 ns       |
| **Serialize Nested Object**   | **2,379 ns** | 2,744 ns    | 5,703 ns          | 6,338 ns              | 6,912 ns               | 8,256 ns     |
| **Deserialize Nested Object** | **1,707 ns** | 1,893 ns    | 3,080 ns          | 2,838 ns              | 3,024 ns               | 3,940 ns     |
| **Roundtrip Serialization**   | **4,209 ns** | 4,791 ns    | 9,099 ns          | 9,378 ns              | 10,075 ns              | 12,203 ns    |
| **Serialize to JSON String**  | **708 ns**   | 769 ns      | 1,366 ns          | 1,415 ns              | 1,541 ns               | 1,815 ns     |
| **Deserialize from JSON**     | **824 ns**   | 863 ns      | 1,606 ns          | 1,619 ns              | 1,840 ns               | 2,371 ns     |

## JSON Parsing

### Parse from String

| Operation                | Linux GCC | Linux Clang  | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------ | --------- | ------------ | ----------------- | --------------------- | ---------------------- | ------------ |
| **Parse Small Object**   | 589 ns    | **498 ns**   | 1,045 ns          | 1,096 ns              | 1,117 ns               | 1,400 ns     |
| **Parse Medium Object**  | 3,170 ns  | **1,801 ns** | 3,601 ns          | 3,986 ns              | 3,990 ns               | 6,619 ns     |
| **Parse Large Object**   | 9,695 ns  | **5,418 ns** | 11,487 ns         | 11,756 ns             | 11,348 ns              | 15,880 ns    |
| **Parse Nested Objects** | 1,776 ns  | **916 ns**   | 1,731 ns          | 1,905 ns              | 1,955 ns               | 2,879 ns     |
| **Parse Small Array**    | 391 ns    | **366 ns**   | 795 ns            | 962 ns                | 1,013 ns               | 1,244 ns     |
| **Parse Large Array**    | 5,871 ns  | **3,823 ns** | 7,547 ns          | 7,782 ns              | 6,880 ns               | 10,542 ns    |
| **Parse Mixed Types**    | 2,189 ns  | **1,410 ns** | 2,911 ns          | 3,078 ns              | 3,058 ns               | 4,160 ns     |

## Schema Generation

### SchemaGenerator Performance

| Operation                   | Linux GCC   | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------- | ----------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Generate from Small Doc** | **5.93 µs** | 6.45 µs     | 12.8 µs           | 14.6 µs               | 16.2 µs                | 17.7 µs      |
| **Generate from Large Doc** | **35.4 µs** | 37.7 µs     | 69.8 µs           | 93.8 µs               | 95.5 µs                | 104.8 µs     |
| **Generate from 10 Docs**   | **78.9 µs** | 81.8 µs     | 147.2 µs          | 301.3 µs              | 232.3 µs               | 261.6 µs     |
| **Generate from 100 Docs**  | **744 µs**  | 787 µs      | 1.48 ms           | 2.26 ms               | 2.36 ms                | 2.58 ms      |
| **Format Inference**        | **17.2 µs** | 17.9 µs     | 31.8 µs           | 42.7 µs               | 44.9 µs                | 49.1 µs      |
| **Constraint Inference**    | **377 µs**  | 397 µs      | 757 µs            | 1.12 ms               | 1.16 ms                | 1.24 ms      |

## Schema Validation

### SchemaValidator Performance

| Operation                   | Linux GCC | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------- | --------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Validate Simple Schema**  | 3.67 µs   | **3.49 µs** | 13.1 µs           | 5.1 µs                | 5.3 µs                 | 6.6 µs       |
| **Validate Complex Schema** | 8.31 µs   | **8.00 µs** | 12.7 µs           | 13.7 µs               | 14.2 µs                | 17.6 µs      |
| **Validate with $ref**      | 4.39 µs   | **4.30 µs** | 6.8 µs            | 6.3 µs                | 6.4 µs                 | 7.9 µs       |
| **Validate String Formats** | 6.85 µs   | **6.49 µs** | 10.1 µs           | 13.6 µs               | 14.8 µs                | 16.4 µs      |
| **Validate with Errors**    | 3.72 µs   | **3.53 µs** | 5.7 µs            | 5.0 µs                | 5.1 µs                 | 6.4 µs       |
| **Validate Deeply Nested**  | 0.72 µs   | **0.67 µs** | 1.1 µs            | 0.9 µs                | 1.0 µs                 | 1.2 µs       |

## JSON Serialization

### Serialize to String

| Operation                  | Linux GCC | Linux Clang  | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| -------------------------- | --------- | ------------ | ----------------- | --------------------- | ---------------------- | ------------ |
| **Serialize Small Object** | 175 ns    | **169 ns**   | 295 ns            | 285 ns                | 292 ns                 | 312 ns       |
| **Serialize Large Object** | 1,186 ns  | **1,161 ns** | 1,480 ns          | 1,651 ns              | 1,709 ns               | 1,838 ns     |
| **Serialize Pretty Print** | 1,310 ns  | **1,286 ns** | 1,693 ns          | 1,860 ns              | 1,898 ns               | 2,028 ns     |

---

_Updated on November 30, 2025_
