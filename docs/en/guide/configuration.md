# Configuration Management

FastQTools supports controlling runtime behavior through configuration files, environment variables, and command-line parameters. The priority from lowest to highest is:

**Defaults → Config File → Environment Variables → Command-Line Parameters**

---

## Command Line Parameters

```bash
# Key-value format
FastQTools stat --input=reads.fq.gz --output=stats.txt

# Separate format
FastQTools stat -i reads.fq.gz -o stats.txt

# Short options
FastQTools stat -i reads.fq.gz -o stats.txt -t 8
```

### Short Option Mapping

| Short | Long | Description |
|-------|------|-------------|
| `-i` | `--input` | Input file |
| `-o` | `--output` | Output file |
| `-t` | `--threads` | Number of threads |
| `-v` | `--verbose` | Verbose logging |
| `-q` | `--quiet` | Quiet mode |

---

## Environment Variables

Environment variables prefixed with `FASTQTOOLS_` are automatically loaded as configuration items (prefix is removed and converted to lowercase):

```bash
export FASTQTOOLS_THREADS=8
export FASTQTOOLS_BATCH_SIZE=100000

FastQTools stat -i input.fastq -o output.txt
# threads=8, batch_size=100000 are automatically applied
```

Command-line parameters can override environment variables:

```bash
export FASTQTOOLS_THREADS=8
FastQTools stat -i input.fastq -o output.txt --threads 16
# Actually uses threads=16
```

---

## Configuration Validation

`Configuration::validate()` performs the following checks:

- Required keys (`input`, `output`) exist
- Value ranges are valid (e.g., `threads` must be between 1–256)
- Configuration consistency

Validation failures throw `fq::error::ConfigurationError` exception.

---

## Build-Time Configuration

### CMake Options

| Option | Description | Default |
|--------|-------------|---------|
| `ENABLE_COVERAGE` | Enable code coverage | OFF |
| `BUILD_BENCHMARKS` | Build benchmarks | OFF |
| `BUILD_TESTING` | Build unit tests | ON |
| `CMAKE_BUILD_TYPE` | Build type | Release |

```bash
cmake -B build -DENABLE_COVERAGE=ON -DBUILD_TESTING=ON
```

### Conan Dependencies

Dependencies are defined in `config/dependencies/conanfile.py`:

| Package | Version | Purpose |
|---------|---------|---------|
| `cxxopts` | 3.1.1 | Command-line parsing |
| `spdlog` | 1.17.0 | Logging |
| `fmt` | 12.1.0 | Formatting |
| `zlib-ng` | 2.3.2 | gzip compression |
| `nlohmann_json` | 3.11.3 | JSON processing |
| `onetbb` | 2022.3.0 | Parallel computing |
| `libdeflate` | 1.25 | High-performance decompression |

---

## Docker Environment Variables

```bash
# Production environment
FASTQTOOLS_DATA_DIR=/app/data
FASTQTOOLS_OUTPUT_DIR=/app/output

# Development environment
CC=clang
CXX=clang++
CONAN_HOME=/home/developer/.conan2
CCACHE_DIR=/home/developer/.ccache
```

---

## Performance Tuning Recommendations

| Parameter | Description | Recommendation |
|-----------|-------------|----------------|
| `threads` | Number of parallel threads | Set to CPU core count |
| `batchSize` | Number of reads per batch | 10000–100000 |
| `batchCapacityBytes` | Batch memory limit | Adjust based on available memory |
| `maxInFlightBatches` | Pipeline concurrent batches | 2× threads |
