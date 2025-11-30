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
| **Get by Key (Top Level)**  | 27.3 ns    | 29.2 ns     | 37.1 ns           | **30.1 ns**           | 32.7 ns                | 39.4 ns      |
| **Get by Key (Nested)**     | 57.9 ns    | **58.8 ns** | 75.2 ns           | 65.9 ns               | 77.3 ns                | 93.5 ns      |
| **Get by Pointer (Top)**    | 30.7 ns    | 31.2 ns     | 33.8 ns           | **29.0 ns**           | 34.1 ns                | 42.2 ns      |
| **Get by Pointer (Nested)** | 67.0 ns    | 67.0 ns     | 76.1 ns           | **68.8 ns**           | 81.5 ns                | 99.9 ns      |
| **Get Deep Nested (3 Lvl)** | 84.7 ns    | **87.3 ns** | 110 ns            | 101 ns                | 108 ns                 | 129 ns       |
| **Get Deep Nested (4 Lvl)** | **108 ns** | **107 ns**  | 142 ns            | 124 ns                | 141 ns                 | 165 ns       |
| **Has Field (Exists)**      | 24.8 ns    | 23.6 ns     | 28.3 ns           | **24.7 ns**           | 29.9 ns                | 32.7 ns      |
| **Has Field (Not Exists)**  | 24.7 ns    | 27.3 ns     | 25.4 ns           | **21.2 ns**           | 25.1 ns                | 28.9 ns      |
| **Has Field (Nested)**      | 127 ns     | 123 ns      | 147 ns            | **123 ns**            | 151 ns                 | 178 ns       |

## JSON Iteration

### Container Traversal

| Operation                   | Linux GCC    | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------- | ------------ | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Object Field Iteration**  | **780 ns**   | 873 ns      | 1,719 ns          | 1,636 ns              | 1,706 ns               | 2,060 ns     |
| **Array Element Iteration** | **7,816 ns** | 8,706 ns    | 18,710 ns         | 19,728 ns             | 19,083 ns              | 21,123 ns    |
| **Nested Object Iteration** | **3,433 ns** | 3,717 ns    | 10,580 ns         | 14,468 ns             | 8,400 ns               | 12,277 ns    |

## JSON Modification

### Document Mutation

| Operation                  | Linux GCC    | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| -------------------------- | ------------ | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Set Primitive (String)** | **105 ns**   | 118 ns      | 260 ns            | 253 ns                | 248 ns                 | 270 ns       |
| **Set Primitive (Int)**    | **95.0 ns**  | 105 ns      | 216 ns            | 226 ns                | 220 ns                 | 239 ns       |
| **Set Primitive (Bool)**   | **93.9 ns**  | 104 ns      | 223 ns            | 229 ns                | 227 ns                 | 243 ns       |
| **Set Primitive (Double)** | **94.7 ns**  | 105 ns      | 224 ns            | 227 ns                | 230 ns                 | 245 ns       |
| **Set Nested (2 Levels)**  | **167 ns**   | 185 ns      | 384 ns            | 372 ns                | 370 ns                 | 414 ns       |
| **Set Nested (3 Levels)**  | **228 ns**   | 255 ns      | 505 ns            | 505 ns                | 486 ns                 | 528 ns       |
| **Set Nested (4 Levels)**  | **291 ns**   | 329 ns      | 626 ns            | 614 ns                | 610 ns                 | 677 ns       |
| **Add Object Field (Seq)** | **654 ns**   | 701 ns      | 2,325 ns          | 1,468 ns              | 1,593 ns               | 2,096 ns     |
| **Append Array Element**   | **202 ns**   | 259 ns      | 439 ns            | 655 ns                | 615 ns                 | 644 ns       |
| **Append to Large Array**  | **1,299 ns** | 1,643 ns    | 1,838 ns          | 3,187 ns              | 3,201 ns               | 3,209 ns     |

## Object Serialization

### Serializer Performance

| Operation                     | Linux GCC    | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ----------------------------- | ------------ | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Serialize POD**             | **278 ns**   | 314 ns      | 550 ns            | 590 ns                | 603 ns                 | 663 ns       |
| **Deserialize POD**           | **74.9 ns**  | 73.7 ns     | 78.4 ns           | 86.5 ns               | 90.1 ns                | 109 ns       |
| **Serialize Medium Object**   | **427 ns**   | 458 ns      | 852 ns            | 997 ns                | 998 ns                 | 1,161 ns     |
| **Deserialize Medium Object** | **143 ns**   | **133 ns**  | 184 ns            | 165 ns                | 169 ns                 | 234 ns       |
| **Serialize Nested Object**   | **2,353 ns** | 2,614 ns    | 5,667 ns          | 6,759 ns              | 6,742 ns               | 7,397 ns     |
| **Deserialize Nested Object** | **1,689 ns** | 1,762 ns    | 2,969 ns          | 3,005 ns              | 3,001 ns               | 3,636 ns     |
| **Roundtrip Serialization**   | **4,179 ns** | 4,512 ns    | 8,905 ns          | 9,902 ns              | 9,812 ns               | 11,196 ns    |
| **Serialize to JSON String**  | **709 ns**   | 742 ns      | 1,299 ns          | 1,499 ns              | 1,521 ns               | 1,683 ns     |
| **Deserialize from JSON**     | **824 ns**   | 804 ns      | 1,504 ns          | 1,715 ns              | 1,775 ns               | 2,125 ns     |

## JSON Parsing

### Parse from String

| Operation                | Linux GCC | Linux Clang  | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------ | --------- | ------------ | ----------------- | --------------------- | ---------------------- | ------------ |
| **Parse Small Object**   | 454 ns    | **492ns**    | 980 ns            | 1,096 ns              | 1,112 ns               | 1,192 ns     |
| **Parse Medium Object**  | 1,820 ns  | **1,770 ns** | 4,507 ns          | 3,918 ns              | 3,953 ns               | 4,776 ns     |
| **Parse Large Object**   | 5,462 ns  | **5,290 ns** | 11,057 ns         | 11,505 ns             | 11,410 ns              | 13,007 ns    |
| **Parse Nested Objects** | 899 ns    | **885 ns**   | 1,727 ns          | 1,960 ns              | 1,933 ns               | 2,469 ns     |
| **Parse Small Array**    | 334 ns    | **356 ns**   | 786 ns            | 988 ns                | 984 ns                 | 1,055 ns     |
| **Parse Large Array**    | 3,693 ns  | **3,757 ns** | 7,401 ns          | 6,772 ns              | 6,762 ns               | 8,164 ns     |
| **Parse Mixed Types**    | 1,390 ns  | **1,389 ns** | 2,808 ns          | 3,092 ns              | 3,069 ns               | 3,576 ns     |

## Schema Generation

### SchemaGenerator Performance

| Operation                   | Linux GCC    | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------- | ------------ | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Generate from Small Doc** | **3.924 µs** | 4.384 µs    | 10.0 µs           | 9.77 µs               | 10.5 µs                | 10.1 µs      |
| **Generate from Large Doc** | **18.3 µs**  | 21.245 µs   | 48.1 µs           | 50.0 µs               | 52.4 µs                | 53.1 µs      |
| **Generate from 10 Docs**   | **43.9 µs**  | 46.684 µs   | 98.3 µs           | 109 µs                | 110 µs                 | 114.7 µs     |
| **Generate from 100 Docs**  | **409 µs**   | 441 µs      | 962 µs            | 1.07 ms               | 1.03 ms                | 1.1 ms       |
| **Format Inference**        | **9.1 µs**   | 9.9 µs      | 23.0 µs           | 24.6 µs               | 24.6 µs                | 26.2 µs      |
| **Constraint Inference**    | **204 µs**   | 222 µs      | 471 µs            | 515.6 µs              | 488 µs                 | 562.5 µs     |

## Schema Validation

### SchemaValidator Performance

| Operation                   | Linux GCC | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------- | --------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **Validate Simple Schema**  | 3.47 µs   | **3.41 µs** | 7.85 µs           | 5.02 µs               | 5.3 µs                 | 5.58 µs      |
| **Validate Complex Schema** | 7.62 µs   | **7.22 µs** | 12.0 µs           | 12.87 µs              | 13.39 µs               | 14.23 µs     |
| **Validate with $ref**      | 4.17 µs   | **4.15 µs** | 6.84 µs           | 6.56 µs               | 6.7 µs                 | 6.8 µs       |
| **Validate String Formats** | 5.17 µs   | **4.92 µs** | 8.37 µs           | 7.5 µs                | 11.0 µs                | 8.02 µs      |
| **Validate with Errors**    | 3.62 µs   | **3.37 µs** | 5.72 µs           | 5.47 µs               | 5.31 µs                | 5.58 µs      |
| **Validate Deeply Nested**  | 701 ns    | **634 ns**  | 1.07 µs           | 1.0 µs                | 1.07 µs                | 1.03 µs      |

## JSON Serialization

### Serialize to String

| Operation                  | Linux GCC | Linux Clang  | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| -------------------------- | --------- | ------------ | ----------------- | --------------------- | ---------------------- | ------------ |
| **Serialize Small Object** | 165 ns    | **164 ns**   | 289 ns            | 297 ns                | 299 ns                 | 289 ns       |
| **Serialize Large Object** | 1,143 ns  | **1,160 ns** | 1,585 ns          | 1,698 ns              | 1,749 ns               | 1,716 ns     |
| **Serialize Pretty Print** | 1,279 ns  | **1,314 ns** | 1,739 ns          | 1,939 ns              | 1,942 ns               | 2,040 ns     |

---

_Updated on November 30, 2025_
