# Design Document

## 1. Design Principles

- **Measurement-driven**: Performance optimization must be based on benchmarks/data.
- **Low complexity first**: Prefer removing inefficiencies through parameters, data structures, and copy path elimination.
- **Reproducible engineering**: Build/test/release processes can be consistently reproduced in CI and locally.

## 2. Data Model and IO

### 2.1 `FastqRecord` and `FastqBatch`

- **Goal**: Reduce string copies and frequent allocations.
- **Implementation**:
  - `fq::io::FastqRecord` uses `std::string_view` to hold field views.
  - `fq::io::FastqBatch` maintains a continuous `buffer_` and `records_` metadata array.
  - Reader parses results into `FastqBatch`, upstream processing only reads/writes `FastqRecord` (views).

### 2.2 Reader/Writer

- Reader: Batch reading and parsing, key parameters:
  - `readChunkBytes` / `zlibBufferBytes` / `maxBufferBytes`
- Writer: Batch writing, key parameters:
  - `zlibBufferBytes` / `outputBufferBytes`

## 3. Processing Pipeline

### 3.1 Serial and Parallel Implementation

- `SequentialProcessingPipeline::run()`: Selects based on `threadCount`:
  - `processSequential()`: Single thread (for debugging, small files)
  - `processWithTBB()`: `tbb::parallel_pipeline`

### 3.2 Parallel Pipeline Pattern (TBB)

- **source (serial_in_order)**: Read next `FastqBatch`.
- **processing (parallel)**: Apply predicates/mutators to batch and collect statistics.
- **sink (serial_in_order)**: Write in order and merge statistics.

### 3.3 Tuning Entry Points (Performance Priority)

- Primary:
  - `batchSize` / `batchCapacityBytes` / `maxInFlightBatches` / `threadCount`
- Secondary (requires evidence):
  - Specialized algorithm optimization, reducing critical path copies
- Not default targets:
  - High-coupling optimizations like SIMD/prefetching/PGO

## 4. Logging

- Uses **global configuration** from `spdlog` (no additional logging framework needed).
- Externally provides `fq::logging::init()` / `setLevel()` as unified entry points; CLI calls after parsing `--log-level`.

## 5. Testing and Quality

### 5.1 Coverage

- `scripts/core/test --coverage`: Generate lcov and HTML.
- CI: Upload coverage artifacts.
- Threshold gating: Reserved as a future option to avoid "gamifying for threshold coverage".

### 5.2 Static Analysis

- Format checking is mandatory.
- clang-tidy/cppcheck: Report first, then converge.

## 6. Release

- `scripts/tools/package-release`: Locally generate `dist/*.tar.gz`.
- `.github/workflows/release.yml`: Tag-triggered, performs packaging and uploads to GitHub Release.
