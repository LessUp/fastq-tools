# Deployment Guide

This document covers deployment options for FastQTools in Docker and production environments.

---

## Docker Deployment

### Production Environment

```bash
# Build production image
./scripts/tools/deploy --env production --action build

# Run
./scripts/tools/deploy --env production --action run

# Or use docker-compose
docker-compose -f docker/docker-compose.yml up -d prod
```

### Development Environment

```bash
# Run development container
./scripts/tools/deploy --env development --action run

# Or use VS Code DevContainer
# Ctrl+Shift+P → "Reopen in Container"
```

### CI/CD

```bash
# Build + Test
./scripts/tools/deploy --env test --action run

# Push to registry
./scripts/tools/deploy --action push --registry your-registry.com/fastqtools --tag v3.1.0
```

---

## Container Images

### Production Image (`docker/Dockerfile.prod`)

- **Base**: debian:bookworm-slim
- **Size**: ~100MB
- **Contents**: FastQTools binary + runtime libraries
- **User**: Non-root (fastqtools:1000)

### Development Image (`docker/Dockerfile.dev`)

- **Base**: gcc:15.2-bookworm
- **Size**: ~2GB
- **Contents**: Full development toolchain
- **User**: Non-root (developer:1000)

---

## Volume Mounts

### Production

```yaml
volumes:
  - ./data:/app/data:ro           # Read-only input
  - ./output:/app/output          # Writable output
```

### Development

```yaml
volumes:
  - ..:/workspace:cached          # Source code
  - ~/.conan2:/home/developer/.conan2:cached  # Conan cache
  - ~/.ccache:/home/developer/.ccache:cached  # Build cache
```

---

## Security

### Production

- Non-root user (UID 1000)
- Minimal base image
- Read-only input volume
- Health check: `fastqtools --help || exit 1`

### Development

- Non-root + sudo
- SSH Agent forwarding (for Git operations)
- Git configuration persistence

---

## Performance Optimization

- **ccache**: Incremental build caching
- **Ninja**: Parallel builds
- **Conan cache**: Avoid recompiling dependencies
- **Multi-stage builds**: Reduce image size
- **Release builds**: `-O3 -march=native`

---

## Health Checks and Logging

```bash
# Health check
docker exec fastqtools-prod FastQTools --help

# View logs
docker logs fastqtools-prod
docker logs -f fastqtools-prod  # Real-time tail
```

---

## Troubleshooting

### Permission Denied

```bash
sudo chown -R 1000:1000 ./data ./output
```

### Missing Libraries

```bash
docker exec fastqtools-prod ldd /usr/local/bin/FastQTools
```

### Debug Mode

```bash
docker run -it --rm fastqtools:latest /bin/bash
docker exec -it fastqtools-prod /bin/bash
```

---

## Version Updates

1. Build new version image
2. Stop old container
3. Start new container with same volumes
4. Verify functionality
5. Remove old image

---

## Best Practices

- Use specific version tags, not `latest`
- Set CPU/memory resource limits
- Use read-only root filesystem where possible
- Regularly update base images
- Monitor container resource usage
