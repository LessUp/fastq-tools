---
title: "/search-codebase"
description: "代码库探索：按功能/模式/接口快速定位代码，理解项目结构和依赖关系。"
auto_execution_mode: 1
---

## 适用场景
- 初次接触项目，需要了解整体结构
- 需要定位某个功能的实现位置
- 追踪调用链或依赖关系

## 工作流步骤
1. **明确搜索目标**
   - 你要找什么？（类/函数/接口/配置/错误消息...）
   - 在哪个范围？（全项目 / 特定模块 / 特定文件类型）

2. **按类型选择搜索策略**

   **找接口/公共 API**：
   - 搜索 `include/fqtools/` 目录
   - `grep -r "class.*{" include/fqtools/`
   - `grep -r "auto.*->" include/fqtools/`（尾置返回类型的函数）

   **找实现**：
   - 搜索 `src/` 目录
   - 对应接口头文件通常在同名子目录

   **找测试**：
   - 搜索 `tests/unit/` 或 `tests/integration/`
   - 测试文件命名：`test_<module>.cpp`

   **找配置/构建**：
   - CMake：`CMakeLists.txt`、`cmake/modules/`
   - Conan：`config/dependencies/conanfile.py`
   - 预设：`CMakePresets.json`

   **找脚本**：
   - 核心：`scripts/core/`
   - 工具：`scripts/tools/`

3. **追踪调用链**
   - 从公共接口 → 实现 → 内部依赖
   - 命名空间映射：
     | 命名空间 | 头文件目录 | 实现目录 |
     |-----------|-----------|---------|
     | `fq::io` | `include/fqtools/io/` | `src/io/` |
     | `fq::processing` | `include/fqtools/processing/` | `src/processing/` |
     | `fq::statistic` | `include/fqtools/statistics/` | `src/statistics/` |
     | `fq::cli` | `include/fqtools/cli/` | `src/cli/` |
     | `fq::config` | `include/fqtools/config/` | `src/config/` |
     | `fq::common` | `include/fqtools/common/` | `src/common/` |

4. **理解模块边界**
   - 接口稳定层：`include/fqtools/` — 修改需谨慎
   - 实现层：`src/` — 可自由修改
   - CLI 层：`src/cli/` — 只依赖接口层
   - 工厂函数：`createProcessingPipeline()`、`createStatisticCalculator()` 是核心入口

## 备注
- 项目使用接口-实现分离模式，公共 API 和实现严格分开
- `fq.h` 是 Façade 入口，聚合所有公共接口
- 命名规范：类 PascalCase、函数 camelCase、私有成员 camelCase + `_`
