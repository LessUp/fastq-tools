---
title: "/benchmark-io"
description: "运行 FastQTools I/O 基准测试（Google Benchmark），并生成报告/回归对比。"
auto_execution_mode: 1
---

## 适用场景
- 你修改了 I/O/压缩/解析相关代码，需要衡量性能影响
- 需要生成 `benchmark_results/` 下的结果与报告

## 工作流步骤
1. **安全前置**
   // turbo
   - `git status -sb`

2. **构建（Release）**
   - `./scripts/core/build -c clang -t Release`

3. **确保已开启 benchmark 构建**
   - 若你是首次跑 benchmark，需在构建目录启用 `BUILD_BENCHMARKS=ON`：
     - 若存在 Conan toolchain：
       - `cmake -S . -B build-clang-release -G Ninja -DCMAKE_TOOLCHAIN_FILE=build-clang-release/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON`
     - 否则：
       - `cmake -S . -B build-clang-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON`
     - `cmake --build build-clang-release --target benchmarks`

4. **生成测试数据**
   - `./scripts/benchmark data generate`

5. **运行 benchmark（保存为 current.json）**
   - `BUILD_DIR=build-clang-release ./scripts/benchmark run --output benchmark_results/results/current.json`

6. **生成报告（Markdown + charts）**
   - `./scripts/benchmark report --input benchmark_results/results/current.json --format markdown --charts`

7. **（可选）回归检查**
   - 若已有基线：
     - `./scripts/benchmark compare benchmark_results/baselines/default.json benchmark_results/results/current.json --ci`

## 产物
- 结果：`benchmark_results/results/*.json`
- 报告：`benchmark_results/reports/`
