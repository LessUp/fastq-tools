# FastQTools Project Structure

## Directory Layout

```
FastQTools/
├── include/fqtools/      # Public API headers (stable interfaces)
├── src/                  # Implementation code
│   ├── cli/              # CLI entry point and command dispatch
│   │   └── commands/     # Subcommand implementations (stat, filter)
│   ├── common/           # Shared utilities
│   ├── config/           # Configuration handling
│   ├── error/            # Error types and handling
│   ├── io/               # FASTQ reading/writing
│   ├── processing/       # Pipeline, mutators, predicates
│   │   ├── mutators/     # Read modification (trimming)
│   │   └── predicates/   # Filtering conditions
│   └── statistics/       # Statistical calculations
├── tests/                # Test suites
│   ├── unit/             # Unit tests by module
│   ├── integration/      # Integration tests
│   └── e2e/              # End-to-end CLI tests
├── scripts/              # Build and dev scripts
│   ├── core/             # Essential scripts (build, test, lint)
│   ├── tools/            # Specialized tools
│   └── lib/              # Shared shell functions
├── tools/                # Development tools
│   ├── benchmark/        # Performance benchmarks
│   └── data/             # Test data files
├── docs/                 # Documentation
│   ├── dev/              # Developer guides
│   ├── user/             # User documentation
│   └── optimization/     # Performance docs
├── config/               # Configuration files
│   ├── conan/            # Conan profiles
│   └── deployment/       # Docker configs
└── cmake/                # CMake modules
```

## Key Files

- `CMakeLists.txt` - Root build configuration
- `conanfile.py` - Dependency specification
- `.clang-format` - Code formatting rules
- `.clang-tidy` - Static analysis config

## Library Modules (CMake targets)

- `fq_common` - Common utilities
- `fq_error` - Error handling
- `fq_config` - Configuration
- `fq_modern_io` - I/O operations
- `fq_processing` - Processing pipeline
- `fq_statistics` - Statistics calculation
- `fq_cli` - CLI commands
- `fq_lib` - Aggregate interface library

## Naming Conventions

| Element | Style | Example |
|---------|-------|---------|
| Source files | `snake_case` | `fastq_reader.cpp` |
| Headers | `snake_case` | `processing_pipeline.h` |
| Classes | `PascalCase` | `FastQReader` |
| Functions | `camelCase` | `processBatch()` |
| Member variables | `camelCase_` | `filePath_` |
| Constants | `kPascalCase` | `kMaxReads` |
| Namespaces | `snake_case` | `fq::utils` |
| Docs | `kebab-case` | `coding-standards.md` |
| Scripts | `snake_case` | `install_deps.sh` |

## Architecture Layers

1. CLI Layer → Argument parsing, command dispatch
2. Command Layer → Business logic orchestration
3. Core Library → Public interfaces (`include/fqtools/`)
4. Implementation → Internal modules (`src/`)
