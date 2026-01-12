# Implementation Plan: Advanced C++ Tooling

## Overview

本实现计划将高级 C++ 开发工具集成到 FastQTools 项目中，包括 Sanitizers、Valgrind、静态分析工具、代码覆盖率和模糊测试支持。

## Tasks

- [ ] 1. Sanitizer 集成
  - [ ] 1.1 创建 CMake Sanitizer 模块
    - 创建 `cmake/modules/fq_sanitizers.cmake`
    - 定义 `ENABLE_ASAN`、`ENABLE_TSAN`、`ENABLE_UBSAN`、`ENABLE_MSAN` 选项
    - 实现 `fq_add_sanitizer_flags()` 函数
    - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.6_
  - [ ] 1.2 在主 CMakeLists.txt 中集成 Sanitizer 模块
    - 包含 `fq_sanitizers.cmake` 模块
    - 对所有目标应用 sanitizer 标志
    - _Requirements: 1.6_
  - [ ] 1.3 更新构建脚本支持 Sanitizer 选项
    - 修改 `scripts/core/build` 添加 `--sanitizer` 参数
    - 支持 asan、tsan、ubsan、msan 选项
    - _Requirements: 1.1, 1.2, 1.3, 1.4_
  - [ ] 1.4 创建 Sanitizer 环境变量配置
    - 创建 `config/sanitizers/asan.options`
    - 创建 `config/sanitizers/tsan.options`
    - 创建 `config/sanitizers/ubsan.options`
    - _Requirements: 1.5_

- [ ] 2. Valgrind 集成
  - [ ] 2.1 创建 Valgrind 抑制文件
    - 创建 `config/valgrind/suppressions.supp`
    - 添加 TBB、spdlog 等第三方库的抑制规则
    - _Requirements: 2.3_
  - [ ] 2.2 更新测试脚本支持 Valgrind
    - 修改 `scripts/core/test` 添加 `--valgrind` 参数
    - 集成抑制文件
    - _Requirements: 2.1, 2.2_
  - [ ] 2.3 创建 Valgrind 工具脚本
    - 创建 `scripts/tools/valgrind-memcheck`
    - 创建 `scripts/tools/valgrind-cachegrind`
    - _Requirements: 2.4_

- [ ] 3. Checkpoint - 验证 Sanitizer 和 Valgrind 集成
  - 确保所有测试通过，如有问题请询问用户

- [ ] 4. 静态分析工具增强
  - [ ] 4.1 创建 Cppcheck 配置
    - 创建 `config/cppcheck/cppcheck.cfg`
    - 创建 `config/cppcheck/suppressions.txt`
    - _Requirements: 3.4_
  - [ ] 4.2 创建 IWYU 配置
    - 创建 `config/iwyu/mappings.imp`
    - _Requirements: 3.2_
  - [ ] 4.3 更新 lint 脚本支持新工具
    - 修改 `scripts/core/lint` 添加 `cppcheck` 子命令
    - 添加 `iwyu` 子命令
    - 更新 `all` 子命令包含 cppcheck
    - _Requirements: 3.1, 3.2, 3.3, 3.5_
  - [ ] 4.4 创建工具检查辅助函数
    - 在 `scripts/lib/common.sh` 中添加 `check_tool()` 函数
    - _Requirements: 3.1, 3.2_

- [ ] 5. 代码覆盖率分析
  - [ ] 5.1 创建 CMake 覆盖率模块
    - 创建 `cmake/modules/fq_coverage.cmake`
    - 定义 `ENABLE_COVERAGE` 选项
    - 实现 `fq_add_coverage_flags()` 函数
    - _Requirements: 4.1_
  - [ ] 5.2 在主 CMakeLists.txt 中集成覆盖率模块
    - 包含 `fq_coverage.cmake` 模块
    - 替换现有的覆盖率配置
    - _Requirements: 4.1_
  - [ ] 5.3 创建覆盖率报告生成脚本
    - 创建 `scripts/tools/coverage-report`
    - 支持 HTML 和 Cobertura XML 输出
    - _Requirements: 4.2, 4.3, 4.4_
  - [ ] 5.4 创建覆盖率阈值配置
    - 创建 `config/coverage/thresholds.json`
    - 在脚本中实现阈值检查
    - _Requirements: 4.5_

- [ ] 6. Checkpoint - 验证静态分析和覆盖率工具
  - 确保所有测试通过，如有问题请询问用户

- [ ] 7. 模糊测试支持
  - [ ] 7.1 创建 CMake 模糊测试模块
    - 创建 `cmake/modules/fq_fuzzing.cmake`
    - 定义 `ENABLE_FUZZING` 选项
    - 实现 `fq_add_fuzzer()` 函数
    - _Requirements: 5.1_
  - [ ] 7.2 创建 FASTQ 解析器 Fuzzer
    - 创建 `tools/fuzz/CMakeLists.txt`
    - 创建 `tools/fuzz/fastq_parser_fuzzer.cpp`
    - _Requirements: 5.2, 5.3_
  - [ ] 7.3 创建模糊测试语料库目录
    - 创建 `tools/fuzz/corpus/` 目录
    - 添加初始种子文件
    - _Requirements: 5.4_
  - [ ] 7.4 创建模糊测试运行脚本
    - 创建 `scripts/tools/run-fuzzer`
    - _Requirements: 5.2_

- [ ] 8. CI/CD 集成
  - [ ] 8.1 创建 GitHub Actions 工作流
    - 创建 `.github/workflows/quality.yml`
    - 配置静态分析检查
    - 配置 ASan 测试
    - 配置覆盖率报告上传
    - _Requirements: 6.1, 6.2, 6.3, 6.4_
  - [ ] 8.2 创建定期 Valgrind 检查工作流
    - 创建 `.github/workflows/valgrind.yml`
    - 配置每周运行
    - _Requirements: 6.5_

- [ ] 9. 开发者文档
  - [ ] 9.1 创建工具使用指南
    - 创建 `docs/dev/quality-tools.md`
    - 包含安装说明、使用示例、输出解读
    - _Requirements: 7.1, 7.2, 7.3, 7.4_
  - [ ] 9.2 更新 tech.md steering 文件
    - 添加新工具的命令说明
    - _Requirements: 7.2_

- [ ] 10. Final Checkpoint - 完整验证
  - 确保所有工具正常工作
  - 确保文档完整
  - 如有问题请询问用户

## Notes

- 所有脚本需要添加工具存在性检查，缺失时给出安装提示
- Sanitizer 之间互斥，不能同时启用多个
- MSan 仅支持 Clang 编译器
- 模糊测试需要 Clang 编译器和 libFuzzer
- 覆盖率分析需要 Debug 构建
