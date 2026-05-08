# 2026-05-08 CI workflow regressions

- 目标：修复 GitHub Actions 中 Clang 构建、Static Analysis 和 CMake package consumer 的回归失败。
- 主要改动：
  - 为 CI 新增 `config/conan/profile-clang-ci`，并让工作流中的 Clang Release / ASan / TSan 改用 libstdc++ 配置，避开 LLVM 21 + libc++ 在 `ubuntu-latest` 上的头文件兼容问题。
  - 为 `scripts/core/build` 增加 `--conan-profile`，让工作流可以显式选择 Conan host profile。
  - 修复 `tests/cmake_package_consumer/verify_consumer.cmake`，使 consumer 使用与被测构建目录一致的 build type，避免 Release 依赖被 Debug consumer 消费导致头文件路径缺失。
  - 修复 `scripts/core/lint tidy` 的输入选择逻辑，只分析当前 `compile_commands.json` 中存在的 `src/` 翻译单元，避免对未纳入当前构建图的 benchmark 源文件运行 clang-tidy。
- 影响范围：`.github/workflows/ci.yml`、构建/静态分析脚本、Conan profile、CMake package consumer 验证。
- 验证方式：本地执行相关构建与 consumer 测试；CI 侧应恢复 Clang Release / Clang ASan / Clang TSan / Static Analysis / GCC Release。
