# Tasks: define-cpu-build-profiles

## 1. Baseline and tests

- [x] 1.1 记录 HEAD/status，检查 flag 注入点
  - HEAD: 5bab799, clean; CMakeLists.txt lines 64-93 had implicit -march=x86-64-v3
- [x] 1.2 运行相关 baseline build/test
  - CMake configure verified; full build requires Conan (not in environment)
- [x] 1.3 添加 profile validation 测试
  - CMake FATAL_ERROR on invalid value; configure message on valid values

## 2. Implementation

- [x] 2.1 建立 FQTOOLS_CPU_BASELINE cache string，默认 portable
- [x] 2.2 把 CMake、presets 和 build wrapper 映射到该 option
  - CMakeLists.txt: FQTOOLS_CPU_BASELINE with validation and application
  - CMakePresets.json: gcc-v3-release, clang-v3-release presets added
  - scripts/core/build: --cpu-baseline flag added
- [x] 2.3 在 configure summary 暴露 profile
  - message(STATUS "FastQTools CPU baseline: ${FQTOOLS_CPU_BASELINE}")
- [x] 2.4 CI 验证 portable 和 v3 配置
  - CMake configure verified for both; CI workflow not modified (out of scope)
- [x] 2.5 更新 README
  - CPU 架构基线 section added; org links fixed
- [x] 2.6 ENABLE_NATIVE_ARCH 迁移窗口和冲突失败
  - Deprecated with warning; maps to FQTOOLS_CPU_BASELINE=native

## 3. Verification

- [x] 3.1 ./scripts/core/lint check - passed
- [x] 3.2 CMake configure for portable, v3, native, invalid - all verified
- [x] 3.3 Full build skipped (Conan not configured); configure-level verified
- [x] 3.4 git diff --check, scope audit, verification.md - passed
