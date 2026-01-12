# Implementation Plan: Benchmark System

## Overview

本实现计划将 Benchmark 系统分解为可执行的编码任务，按照依赖关系排序。实现采用 C++ 和 Python 混合方案：C++ 负责核心基准测试执行，Python 负责报告生成和可视化。

## Tasks

- [x] 1. 项目结构和基础设施
  - [x] 1.1 创建 benchmark 系统目录结构
    - 创建 `benchmark_results/` 目录结构（results/, baselines/, reports/, data/）
    - 更新 `.gitignore` 排除临时结果文件
    - _Requirements: 2.4_

  - [x] 1.2 扩展 CMakeLists.txt 支持新的 benchmark 目标
    - 添加 `benchmark_full` 目标，包含所有基准测试
    - 配置 JSON 输出选项
    - _Requirements: 1.1_

- [x] 2. 数据模型和序列化
  - [x] 2.1 实现 BenchmarkResult 和 BenchmarkReport 数据结构
    - 在 `include/fqtools/benchmark/types.h` 中定义数据结构
    - 包含所有必需字段：name, category, iterations, mean_time_ns, std_dev_ns, throughput_mbps, peak_memory_bytes, thread_count, input_size
    - _Requirements: 2.2_

  - [x] 2.2 实现 SystemMetadata 数据结构和收集器
    - 实现 CPU、内存、OS 信息收集
    - 实现 Git commit 和 branch 获取
    - _Requirements: 2.3_

  - [x] 2.3 实现 JSON 序列化/反序列化
    - 使用 nlohmann-json 实现 to_json/from_json
    - 确保所有必需字段都被序列化
    - _Requirements: 2.1_

  - [ ]* 2.4 编写属性测试：JSON 输出完整性
    - **Property 1: JSON Output Completeness**
    - **Validates: Requirements 2.1, 2.2, 2.3**

- [x] 3. Checkpoint - 确保数据模型测试通过
  - 确保所有测试通过，如有问题请询问用户

- [x] 4. 基准测试执行器
  - [x] 4.1 重构现有 fastq_io_benchmark.cpp
    - 添加多线程配置支持
    - 添加多种输入大小支持
    - 输出结构化结果
    - _Requirements: 1.2, 1.6, 1.7_

  - [x] 4.2 实现 Filter 命令基准测试
    - 创建 `filter_benchmark.cpp`
    - 测试不同过滤配置的性能
    - _Requirements: 1.4_

  - [x] 4.3 实现 Stat 命令基准测试
    - 创建 `stat_benchmark.cpp`
    - 测试不同文件大小的分析吞吐量
    - _Requirements: 1.5_

  - [x] 4.4 实现错误处理和恢复机制
    - 捕获单个 benchmark 失败
    - 记录错误并继续执行其他 benchmark
    - _Requirements: 1.8_

  - [ ]* 4.5 编写属性测试：指标收集完整性
    - **Property 2: Metric Collection Completeness**
    - **Validates: Requirements 1.2, 1.3, 1.4, 1.5**

  - [ ]* 4.6 编写属性测试：配置传播
    - **Property 3: Configuration Propagation**
    - **Validates: Requirements 1.6, 1.7**

  - [ ]* 4.7 编写属性测试：错误恢复
    - **Property 13: Error Resilience**
    - **Validates: Requirements 1.8**

- [x] 5. Checkpoint - 确保基准测试执行器测试通过
  - 确保所有测试通过，如有问题请询问用户

- [x] 6. 结果存储和历史管理
  - [x] 6.1 实现结果文件命名和存储
    - 格式：`YYYY-MM-DD_HH-MM-SS_<commit>.json`
    - 存储到 `benchmark_results/results/`
    - _Requirements: 2.4, 2.5_

  - [x] 6.2 实现基线管理功能
    - 保存当前结果为命名基线
    - 列出所有可用基线
    - 加载指定基线
    - _Requirements: 6.1, 6.2, 6.3, 6.4_

  - [ ]* 6.3 编写属性测试：历史数据保留
    - **Property 4: Historical Data Preservation**
    - **Validates: Requirements 2.4, 2.5**

  - [ ]* 6.4 编写属性测试：基线管理
    - **Property 12: Baseline Management**
    - **Validates: Requirements 6.2, 6.4, 6.5**

- [x] 7. 测试数据生成器
  - [x] 7.1 增强现有 gen_fastq.py 脚本
    - 添加更多配置选项（质量分布、N 比例）
    - 支持生成压缩文件
    - _Requirements: 7.2, 7.3_

  - [x] 7.2 实现输入文件验证
    - 验证 FASTQ 格式正确性
    - 支持 .gz 和未压缩文件
    - _Requirements: 7.4, 7.5_

  - [ ]* 7.3 编写属性测试：数据生成器正确性
    - **Property 7: Data Generator Correctness**
    - **Validates: Requirements 7.2, 7.3**

  - [ ]* 7.4 编写属性测试：输入验证
    - **Property 8: Input Validation**
    - **Validates: Requirements 7.4**

  - [ ]* 7.5 编写属性测试：文件格式支持
    - **Property 9: File Format Support**
    - **Validates: Requirements 7.5**

- [x] 8. Checkpoint - 确保数据生成和验证测试通过
  - 确保所有测试通过，如有问题请询问用户

- [x] 9. 报告生成器 (Python)
  - [x] 9.1 创建 Python 报告生成器框架
    - 创建 `tools/benchmark/report_generator.py`
    - 实现 JSON 结果加载
    - _Requirements: 3.1_

  - [x] 9.2 实现 Markdown 报告生成
    - 生成性能摘要表格
    - 生成配置对比表格
    - _Requirements: 3.3, 3.6_

  - [x] 9.3 实现图表生成
    - 使用 matplotlib 生成吞吐量趋势图
    - 生成线程扩展性图
    - 输出 SVG/PNG 格式
    - _Requirements: 3.1, 3.2, 3.4_

  - [x] 9.4 实现 README 嵌入功能
    - 生成可嵌入的性能摘要
    - 实现文档更新（保留现有内容）
    - _Requirements: 8.1, 8.2, 8.3_

  - [ ]* 9.5 编写属性测试：Markdown 报告有效性
    - **Property 10: Markdown Report Validity**
    - **Validates: Requirements 3.3, 3.6**

  - [ ]* 9.6 编写属性测试：文档更新保留
    - **Property 11: Documentation Update Preservation**
    - **Validates: Requirements 8.3, 8.4**

- [x] 10. 回归检测器
  - [x] 10.1 实现回归检测逻辑
    - 创建 `tools/benchmark/regression_detector.py`
    - 实现阈值比较（10% warning, 20% critical）
    - 支持自定义阈值
    - _Requirements: 4.1, 4.2, 4.5_

  - [x] 10.2 实现回归报告输出
    - 生成详细的指标对比
    - 输出 CI 友好格式
    - _Requirements: 4.3, 4.4_

  - [ ]* 10.3 编写属性测试：回归检测阈值
    - **Property 5: Regression Detection Thresholds**
    - **Validates: Requirements 4.1, 4.2, 4.4, 4.5**

- [x] 11. Checkpoint - 确保报告和回归检测测试通过
  - 确保所有测试通过，如有问题请询问用户

- [x] 12. CLI 和 CI 集成
  - [x] 12.1 创建统一的 benchmark CLI 脚本
    - 创建 `scripts/benchmark` 主入口
    - 实现 run/report/compare/baseline 子命令
    - _Requirements: 5.1_

  - [x] 12.2 实现 CI 模式
    - 添加 `--ci` 标志
    - 实现 GitHub Actions 注解输出
    - 实现正确的退出码
    - _Requirements: 5.2, 5.3, 5.4_

  - [x] 12.3 创建 GitHub Actions workflow
    - 创建 `.github/workflows/benchmark.yml`
    - 配置定期运行和 PR 触发
    - _Requirements: 5.1_

  - [x] 12.4 实现性能徽章生成
    - 生成 shields.io 兼容的徽章
    - _Requirements: 5.5_

  - [ ]* 12.5 编写属性测试：CI 退出码一致性
    - **Property 6: CI Exit Code Consistency**
    - **Validates: Requirements 5.2, 5.4**

- [x] 13. 文档和集成
  - [x] 13.1 创建性能报告文档页面
    - 创建 `docs/performance/benchmark-report.md`
    - 包含方法论和系统规格说明
    - _Requirements: 8.1, 8.4_

  - [x] 13.2 更新 README.md 添加性能部分
    - 添加性能摘要部分
    - 添加性能徽章
    - _Requirements: 8.2_

  - [x] 13.3 创建 benchmark 使用文档
    - 创建 `docs/dev/benchmark-guide.md`
    - 说明如何运行和解读 benchmark
    - _Requirements: 8.4_

- [x] 14. Final Checkpoint - 确保所有测试通过
  - 运行完整测试套件
  - 验证 CI 集成工作正常
  - 确保所有测试通过，如有问题请询问用户

## Notes

- Tasks marked with `*` are optional and can be skipped for faster MVP
- Each task references specific requirements for traceability
- Checkpoints ensure incremental validation
- Property tests validate universal correctness properties
- C++ 部分使用 Google Test，Python 部分使用 pytest + hypothesis
