# Build Guide

The build system exists to keep local development, CI verification, and performance testing on the same set of entry points so individual contributors do not drift into incompatible command habits.

## Recommended entry points

Use `scripts/core/*` from the repository root:

```bash
./scripts/core/build
./scripts/core/build --dev
./scripts/core/build --compiler gcc --type Debug
./scripts/core/build --sanitizer asan
```

These scripts coordinate Conan, CMake presets, and the default build directories so maintainers do not need to remember low-level environment detail for routine work.

## Common build scenarios

| Scenario | Recommended command | Meaning |
| --- | --- | --- |
| Daily validation | `./scripts/core/build --dev` | Produces a build better suited for debugging |
| Pre-release check | `./scripts/core/build` | Default Release build |
| Sanitizer debugging | `./scripts/core/build --sanitizer asan` | Helps locate memory and undefined-behavior issues |
| Coverage work | `./scripts/core/build --coverage` | Pairs with the test scripts for coverage data |

## Directories and artifacts

The default build directories follow `build/<preset>`, for example `build/clang-debug` and `build/clang-release`. Maintenance scripts, CI, and docs should reuse that convention instead of assuming custom paths.

## When to drop lower

Only hand-write `conan install`, `cmake --preset`, or target-level build commands when debugging Conan, presets, or a specific target. Day-to-day work should prefer the script entry points because they are part of the repository’s standard workflow.

## What to do after a build

A successful build only proves the code compiles. If the change affects behavior, continue with the relevant verification from [`Testing`](./testing). If it affects the performance narrative, review the [`Benchmark Guide`](./benchmark-guide) too.
