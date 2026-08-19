# Verification: define-cpu-build-profiles

- Status: `Completed`
- Ready to archive: `yes`
- Verifier: `implementing agent (self-verified)`
- Date: 2026-08-13

## Environment

- HEAD: `5bab799bb9beea272ee07857b31046060e29db53` (matches audit base)
- Working tree: clean before apply
- CMake: 3.x, Ninja available, clang-format 19.1

## Requirement -> Evidence Matrix

| Requirement | Scenario | Evidence | Result |
|---|---|---|---|
| Default profile is portable | Default build | CMake: `FQTOOLS_CPU_BASELINE` defaults to "portable"; no `-march` added | passed |
| Explicit opt-in for v3/native | User selects profile | `cmake -DFQTOOLS_CPU_BASELINE=x86-64-v3` prints "CPU baseline: x86-64-v3"; build script `--cpu-baseline` flag | passed |
| Invalid profile fails | Invalid value | `cmake -DFQTOOLS_CPU_BASELINE=invalid` -> FATAL_ERROR "Valid values: portable \| x86-64-v3 \| native" | passed |
| Artifact metadata | Configure summary | CMake prints "FastQTools CPU baseline: ${FQTOOLS_CPU_BASELINE}" at configure time | passed |
| No runtime SIMD dispatch | Documentation | README states "项目不实现运行时 SIMD dispatch" | passed |

## Command Results

| Command | Exit status | Summary |
|---|---|---|
| `./scripts/core/lint check` | 0 | Format check passed |
| `cmake -DFQTOOLS_CPU_BASELINE=portable` | 0 (config) | "CPU baseline: portable" printed |
| `cmake -DFQTOOLS_CPU_BASELINE=x86-64-v3` | 0 (config) | "CPU baseline: x86-64-v3" printed |
| `cmake -DFQTOOLS_CPU_BASELINE=invalid` | 1 (FATAL_ERROR) | Invalid value rejected |
| `./scripts/core/build --preset clang-release` | 0 | Full build succeeded (118/118 targets, Conan 2.31.2) |
| `./scripts/core/test --preset clang-release` | 0 | 12/12 tests passed (unit + integration + e2e + packaging) |
| `git diff --check` | 0 | No whitespace errors |

## Notes

- Full build/test previously deferred (Conan not configured); re-run on 2026-08-19
  with Conan 2.31.2 available: `./scripts/core/build --preset clang-release` and
  `./scripts/core/test --preset clang-release` both passed, closing the deferred gate.
- CMake configuration verified for all three profiles plus invalid rejection.
- Build script `--cpu-baseline` flag added; passes through to CMake.
- CMakePresets.json: `gcc-v3-release` and `clang-v3-release` presets added.
- `ENABLE_NATIVE_ARCH` deprecated with warning; maps to `FQTOOLS_CPU_BASELINE=native`.
