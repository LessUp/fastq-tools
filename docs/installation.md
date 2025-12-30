# FastQTools Installation Guide

This guide covers different installation methods for FastQTools, from development setup to production deployment.

## Quick Start

### For End Users (Runtime Only)

If you just want to run FastQTools without developing it:

```bash
# Install runtime dependencies only
./scripts/install_runtime.sh

# Or use the main script with runtime flag
./scripts/install_deps.sh --runtime
```

### For Developers

If you want to build and develop FastQTools:

```bash
# Install development dependencies (includes runtime)
./scripts/install_deps.sh --dev

# Or install everything
./scripts/install_deps.sh --all
```

## Installation Scripts

### `scripts/install_deps.sh`

Main installation script with multiple modes:

- `--runtime`: Install only runtime dependencies (minimal)
- `--dev` or `--development`: Install development dependencies (default)
- `--all`: Install both runtime and development dependencies

**Runtime Dependencies:**
- ca-certificates
- libtbb12 (Intel Threading Building Blocks)
- zlib1g (compression library)
- libbz2-1.0 (bzip2 compression)
- liblzma5 (LZMA compression)
- libdeflate0 (deflate compression)

**Development Dependencies:**
- All runtime dependencies
- build-essential (GCC compiler suite)
- cmake, ninja-build (build systems)
- clang-19, clangd-19, clang-tidy-19, clang-format-19 (LLVM toolchain)
- python3-pip, conan (dependency management)
- gdb, valgrind (debugging tools)
- lcov (code coverage)
- doxygen, graphviz (documentation)

### `scripts/install_runtime.sh`

Lightweight script for production environments that only installs runtime dependencies.

## Docker Deployment

### `scripts/docker_deploy.sh`

Comprehensive Docker deployment script supporting multiple environments:

```bash
# Build production image
./scripts/docker_deploy.sh --env production --action build

# Run development environment
./scripts/docker_deploy.sh --env development --action run

# Run tests in container
./scripts/docker_deploy.sh --env test --action run

# Push to registry
./scripts/docker_deploy.sh --action push --registry myregistry.com/fastqtools --tag v3.1.0
```

**Supported environments:**
- `production`: Optimized runtime image
- `development`: Full development environment with tools
- `test`: Testing environment with coverage tools

**Supported actions:**
- `build`: Build Docker image
- `run`: Build and run container
- `stop`: Stop and remove container
- `push`: Push image to registry

### Docker Compose

Use docker-compose for multi-service deployments:

```bash
# Development environment
docker-compose -f docker/docker-compose.yml up -d dev

# Production environment
docker-compose -f docker/docker-compose.yml up -d prod

# Run tests
docker-compose -f docker/docker-compose.yml up test
```

## Development Container

### VS Code DevContainer

Two devcontainer configurations are available:

1. **Full setup** (`.devcontainer/devcontainer.json`): Uses docker-compose
2. **Simple setup** (`.devcontainer/devcontainer.simple.json`): Single Dockerfile

Both provide:
- Clang/LLVM 19 toolchain
- CMake and Ninja build systems
- Conan package manager
- VS Code extensions for C++ development

## System Requirements

### Minimum Requirements (Runtime)
- Ubuntu 20.04+ or Debian 11+
- 1GB RAM
- 100MB disk space

### Recommended Requirements (Development)
- Ubuntu 22.04+ or Debian 12+
- 4GB RAM
- 2GB disk space
- Multi-core CPU for parallel builds

## Troubleshooting

### Common Issues

1. **Missing libdeflate**: Ensure you're using Ubuntu 22.04+ or install from source
2. **Clang not found**: Run `sudo update-alternatives --config clang` to set default
3. **Conan profile**: Run `conan profile detect --force` after installation
4. **Permission errors**: Ensure your user is in the `docker` group for Docker commands

### Verification

After installation, verify your setup:

```bash
# Check runtime libraries
ldconfig -p | grep -E "(tbb|deflate|lzma|bz2)"

# Check development tools (if installed)
clang --version
cmake --version
conan --version

# Test FastQTools (after building)
./build/FastQTools --help
```

## Next Steps

After installation:

1. **Build the project**: See [Building Guide](building.md)
2. **Run tests**: See [Testing Guide](testing.md)
3. **Development workflow**: See [Development Guide](development.md)