# Deployment

The key to deploying FastQTools is not forcing every scenario into one platform abstraction. It is choosing the smallest maintainable option for the runtime boundary you actually have: local scripts, container packaging, or CI and batch pipelines.

## Scenario 1: run directly on a local or shared node

This is the most direct option and works well for development, debugging, and smaller-scale batch processing:

```bash
./scripts/core/install-deps --runtime
./scripts/core/build
./build/clang-release/FastQTools --help
```

The advantage is transparency and low debugging cost. The trade-off is that dependencies stay more tightly coupled to the host machine.

## Scenario 2: containerized delivery

If you need a more stable runtime boundary, prefer the Docker entry points already in the repository:

- `docker/Dockerfile.dev`: for development and debugging
- `docker/Dockerfile.prod`: for a slim runtime
- `docker/docker-compose.yml`: for common service and mount composition

The normal pattern is to mount input directories read-only, mount output directories separately as writable, and keep the container command identical to the local CLI command.

## Scenario 3: CI and automation

In CI, a useful default breakdown is:

1. Build: `./scripts/core/build` or `./scripts/core/build --dev`
2. Verify: `./scripts/core/test`
3. Documentation or release extras: benchmark, docs build, or image steps as needed

For pure data-processing pipelines, you often do not need another platform wrapper around the command itself. Using `FastQTools stat ...` or `FastQTools filter ...` directly as an auditable step is usually clearer.

## Decisions worth making before deployment

| Decision point | Question to answer |
| --- | --- |
| Input/output boundary | Where does data come from and where do results go? |
| Concurrency policy | Is thread count controlled by the host, scheduler, or script? |
| Logging policy | Do you need debug information or only error output? |
| Evidence retention | Will you keep `stat` output, sidecar files, or benchmark records as well? |

## Relationship to other pages

- If you have not run a command yet, go back to [`Getting Started`](./getting-started)
- If you are looking up specific flags, return to [`CLI Reference`](./cli-reference)
- If you care about build and test maintenance details, continue to [`Build Guide`](../dev/build) and [`Testing`](../dev/testing)
