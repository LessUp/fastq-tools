# Coding Standards

## Style Origins

- This repository's coding style and tool configurations (`.clang-format`, `.clang-tidy`, `.editorconfig`, `.git-blame-ignore-revs`) are **adapted from and inspired by** the MongoDB repository, aiming to establish an engineering-oriented and maintainable C++ development standard.
- **Note**: This project is not a fully consistent implementation of MongoDB's official repository standards, but rather a tailored and optimized version based on modern C++23 features (such as trailing return types) and this project's specific requirements.

## Basic Rules

- **Language Standard**: C++23
- **Compiler**: GCC 15 / modern Clang (local Clang builds are automatically adapted by scripts to installed version via Conan)
- **Formatting**: Use `.clang-format` for automatic formatting (based on LLVM style, 4-space indentation, 100-column width)

### Manual Unified Commands

- Full format: `./scripts/core/lint format`
- Check only: `./scripts/core/lint check`

## Naming Conventions

| Element | Style | Example |
|---------|-------|---------|
| Code files/directories | `snake_case` | `file_parser.h`, `src/utils/` |
| Documentation | `kebab-case` | `docs/dev/git-guidelines.md` |
| Scripts | `kebab-case` | `scripts/core/install-deps` |
| Classes/Structs | `PascalCase` | `class FastQReader;` |
| Functions/Methods | `camelCase` | `void processBatch();` |
| Variables (local/params) | `camelCase` | `int readCount = 0;` |
| Class member variables | `camelCase` with **trailing underscore** (e.g., `member_`, `impl_`) | `filePath_` |
| Constants | `kConstantName` | `constexpr int kMaxReads = 1000;` |
| Namespaces | `snake_case` | `namespace fq::utils` |

> Documentation and scripts use `kebab-case` naming (e.g., `install-deps`, `benchmark-io`).

### Naming Exceptions (Repository Conventions)

The following file names are industry conventions/tool requirements and are not subject to `snake_case` constraints:

- `README.md` / `CHANGELOG.md` / `LICENSE` / `CODE_OF_CONDUCT.md` / `CONTRIBUTING.md` / `SECURITY.md`
- `CMakeLists.txt` / `*.cmake` / `*.cmake.in`
- `Dockerfile` / `docker-compose.yml`
- `conanfile.py` / `package.json`

The following are generated artifacts/directories and default to not participating in naming convention checks:

- `build/` / `build-*` / `cmake-build-*` / `_cmake_install_prefix/` / `_output/`

## Code Organization

### Directory Structure

```
include/fqtools/     # Public API headers
src/                 # Internal implementation code
src/cli/             # Command-line entry
src/cli/commands/    # Subcommand implementations
src/processing/      # Processing logic (Mutators, Predicates)
src/statistics/      # Statistics logic
src/io/              # Input/output processing
```

### Include Order

1. C++ standard library
2. Third-party libraries
3. Other modules from this project

### Interface Design

- Headers contain only declarations
- Implementation details go in source files
- Use `#pragma once` to prevent duplicate inclusion

## Modern C++ Practices

### Resource Management

```cpp
// Prefer smart pointers
std::unique_ptr<Processor> processor = create_processor();
std::shared_ptr<Config> config = std::make_shared<Config>();

// No raw new/delete
```

### Function Declarations

```cpp
// Use trailing return type consistently
auto processBatch(const Batch& batch) -> Result;
```

### Error Handling

```cpp
// Prefer exceptions
if (!is_valid(input)) {
    throw InvalidInputError("Invalid input file");
}
```

### String Handling

```cpp
// Prefer string_view for read-only parameters
auto process_sequence(std::string_view sequence) -> bool;
```

## CLI Standards

### Command Naming

- Subcommands: Short verbs or nouns (`stat`, `filter`)
- Command classes: `PascalCase` + `Command` suffix (`StatCommand`)

### Option Format

- Long options: `kebab-case` (`--input`, `--min-quality`)
- Short options: Single letter (`-i`, `-o`, `-t`)
- Boolean switches: Positive options, with `--no-*` when needed

### Global Options

- `-v, --verbose`: Verbose logging
- `-q, --quiet`: Only errors output
- `--log-level=LEVEL`: Explicitly set log level

## Tool Scripts

### Script Standards

```bash
#!/bin/bash
set -euo pipefail  # Strict mode

# Function naming: snake_case
print_info() {
    echo "INFO: $*"
}

# Variable references: double quotes
run_build "$compiler" "$build_type"
```

### Unified Tools

- `scripts/core/lint`: Code quality checks (clang-tidy + clang-format)
- `scripts/core/build`: Build management
- `scripts/core/test`: Test execution

## Documentation Requirements

### API Documentation

```cpp
/**
 * @brief Process FASTQ batch data
 * @param batch The batch data to process
 * @return Processing result
 * @throws ProcessingError Thrown when processing fails
 */
auto processBatch(const Batch& batch) -> Result;
```

### Code Comments

- Add necessary comments for complex logic
- Avoid obvious comments
- Keep comments synchronized with code
