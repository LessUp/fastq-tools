# FastQTools Deployment Guide

## Overview

This document provides a comprehensive guide for deploying FastQTools in different environments, from development to production.

## Installation Options

### 1. Runtime-Only Installation (End Users)

For users who just want to run FastQTools:

```bash
# Minimal runtime dependencies
./scripts/install_runtime.sh

# Or using the main script
./scripts/install_deps.sh --runtime
```

**What gets installed:**
- Essential runtime libraries (TBB, zlib, bzip2, LZMA, libdeflate)
- No development tools
- Minimal disk footprint (~50MB)

### 2. Development Installation

For developers who want to build and modify FastQTools:

```bash
# Full development environment
./scripts/install_deps.sh --dev

# Or install everything
./scripts/install_deps.sh --all
```

**What gets installed:**
- All runtime dependencies
- Complete LLVM/Clang 19 toolchain
- CMake, Ninja build systems
- Conan package manager
- Debugging and profiling tools
- Documentation tools

## Docker Deployment

### Production Deployment

```bash
# Build optimized production image
./scripts/docker_deploy.sh --env production --action build

# Run production container
./scripts/docker_deploy.sh --env production --action run

# Or use docker-compose
docker-compose -f docker/docker-compose.yml up -d prod
```

### Development Environment

```bash
# Run development container with full toolchain
./scripts/docker_deploy.sh --env development --action run

# Or use VS Code DevContainer
# Open in VS Code and select "Reopen in Container"
```

### CI/CD Pipeline

```bash
# Build and test
./scripts/docker_deploy.sh --env test --action run

# Push to registry
./scripts/docker_deploy.sh --action push --registry your-registry.com/fastqtools --tag v3.1.0
```

## Container Images

### Production Image (`docker/Dockerfile`)
- **Base**: debian:12-slim
- **Size**: ~100MB
- **Contents**: FastQTools binary + runtime libraries only
- **User**: Non-root (fastqtools:1000)
- **Security**: Minimal attack surface

### Development Image (`docker/Dockerfile.dev`)
- **Base**: gcc:14.2-bookworm
- **Size**: ~2GB
- **Contents**: Full development toolchain
- **User**: Non-root (developer:1000)
- **Features**: All development tools, debugging capabilities

## Environment Variables

### Production
```bash
FASTQTOOLS_DATA_DIR=/app/data      # Input data directory
FASTQTOOLS_OUTPUT_DIR=/app/output  # Output directory
```

### Development
```bash
CC=clang                          # C compiler
CXX=clang++                       # C++ compiler
CONAN_HOME=/home/developer/.conan2 # Conan cache
CCACHE_DIR=/home/developer/.ccache # Build cache
```

## Volume Mounts

### Production
```yaml
volumes:
  - ./data:/app/data:ro           # Read-only input data
  - ./output:/app/output          # Writable output directory
```

### Development
```yaml
volumes:
  - ..:/workspace:cached          # Source code (cached for performance)
  - ~/.conan2:/home/developer/.conan2:cached  # Conan cache
  - ~/.ccache:/home/developer/.ccache:cached  # Build cache
```

## Security Considerations

### Production
- Runs as non-root user (UID 1000)
- Minimal base image (debian:12-slim)
- No development tools in production image
- Read-only input volumes
- Health checks enabled

### Development
- Non-root user with sudo access
- SSH key forwarding for Git operations
- Git configuration persistence
- Isolated from host system

## Performance Optimization

### Build Performance
- ccache for incremental builds
- Ninja build system (parallel builds)
- Conan binary cache
- Multi-stage Docker builds

### Runtime Performance
- Optimized release builds (-O3 -march=native)
- Intel TBB for parallelization
- libdeflate for fast compression
- SIMD optimizations where available

## Monitoring and Health Checks

### Docker Health Check
```dockerfile
HEALTHCHECK --interval=30s --timeout=5s --start-period=5s --retries=3 \
    CMD fastqtools --help || exit 1
```

### Container Logs
```bash
# View logs
docker logs fastqtools-prod

# Follow logs
docker logs -f fastqtools-prod
```

## Troubleshooting

### Common Issues

1. **Permission Denied**
   ```bash
   # Fix file permissions
   sudo chown -R 1000:1000 ./data ./output
   ```

2. **Missing Dependencies**
   ```bash
   # Verify runtime libraries
   docker exec fastqtools-prod ldd /usr/local/bin/FastQTools
   ```

3. **Build Failures**
   ```bash
   # Check development environment
   docker exec fastqtools-dev clang --version
   docker exec fastqtools-dev conan --version
   ```

### Debug Mode

```bash
# Run container in debug mode
docker run -it --rm fastqtools:latest /bin/bash

# Or attach to running container
docker exec -it fastqtools-prod /bin/bash
```

## Best Practices

### Production
- Use specific version tags, not `latest`
- Set resource limits (CPU, memory)
- Use read-only root filesystem where possible
- Regular security updates of base images
- Monitor container resource usage

### Development
- Use volume caches for better performance
- Keep development and production images in sync
- Regular cleanup of unused images and containers
- Use multi-stage builds to reduce image size

## Migration Guide

### From Manual Installation
1. Stop existing FastQTools processes
2. Backup configuration and data
3. Deploy using Docker
4. Migrate configuration to environment variables
5. Test functionality

### Version Updates
1. Build new image with updated tag
2. Stop old container
3. Start new container with same volumes
4. Verify functionality
5. Remove old image

## Support

For deployment issues:
1. Check container logs
2. Verify system requirements
3. Test with minimal configuration
4. Consult troubleshooting section
5. Report issues with full environment details