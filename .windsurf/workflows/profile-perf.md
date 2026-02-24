---
title: "/profile-perf"
description: "性能剖析：perf/flamegraph 采集与分析，定位 CPU 热点和性能瓶颈。"
auto_execution_mode: 1
---

## 适用场景
- 程序运行慢，需要定位 CPU 热点
- 优化前的性能基线采集
- 对比优化前后的性能差异

## 工作流步骤
1. **用 Release 模式构建（保留符号）**
   - `./scripts/core/build -c clang -t Release`
   - Release 构建已启用 `-O2`/`-O3` + LTO，接近生产性能

2. **准备测试数据**
   - 确保有足够大的输入文件（推荐 ≥100MB）
   - 可用 `python3 tools/data/gen_fastq.py` 生成

3. **perf 采集**
   - **CPU 采样**：
     ```bash
     perf record -g --call-graph dwarf -F 99 \
       ./build-clang-release/FastQTools stat <large_input.fq.gz>
     ```
   - **统计概览**：
     ```bash
     perf stat ./build-clang-release/FastQTools stat <large_input.fq.gz>
     ```

4. **分析 perf 数据**
   - **热点函数**：`perf report --sort=dso,symbol`
   - **调用链**：`perf report -g`
   - **注解源码**：`perf annotate <function_name>`

5. **生成火焰图（可选）**
   - 需要安装 FlameGraph 工具：`git clone https://github.com/brendangregg/FlameGraph.git`
   - 生成：
     ```bash
     perf script | FlameGraph/stackcollapse-perf.pl | FlameGraph/flamegraph.pl > flamegraph.svg
     ```
   - 在浏览器中打开 `flamegraph.svg` 交互分析

6. **Valgrind Cachegrind 补充分析（可选）**
   - 如果需要缓存命中率分析：`/valgrind-cachegrind`
   - 适用于内存密集型瓶颈

7. **对比优化效果**
   - 用 Google Benchmark 做量化对比：`./scripts/tools/benchmark run`
   - 或用 `./scripts/tools/benchmark compare baseline.json current.json`

## 常用 perf 命令速查
| 命令 | 用途 |
|------|------|
| `perf stat <cmd>` | 硬件计数器概览（cycles, instructions, cache-misses） |
| `perf record -g <cmd>` | 采样记录（带调用链） |
| `perf report` | 交互式分析 |
| `perf report --sort=dso,symbol` | 按共享库+函数排序 |
| `perf annotate <func>` | 源码级热点标注 |
| `perf diff old.data new.data` | 对比两次采样 |

## 备注
- 确保系统允许 perf 采集：`echo 0 > /proc/sys/kernel/perf_event_paranoid`（需 root）
- 如果符号缺失，确认构建时带 `-g` 并且未 strip
- TBB 并行管道的热点可能分散在多个 worker 线程中，注意查看所有线程
