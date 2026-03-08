# 2025-12-25 遗留代码与头文件清理

> 本文件合并自 5 个同日同主题的独立 changelog 条目。

## 1. 移除 src/modules/pipeline 残留目录

### 背景
- 仓库已存在新的流水线实现目录 `src/pipeline/`。
- `src/modules/pipeline/` 仅残留一个 `CMakeLists.txt`，未被 CMake 引入、无源码文件。

### 变更
- 删除 `src/modules/pipeline/CMakeLists.txt`。

### 影响
- 仅清理无引用的残留构建入口，不影响现有编译与运行。

---

## 2. 删除 legacy io/fastq 模块（迁移到 fqtools/io/）

### 背景
- 仓库同时存在两套 FASTQ/IO 实现：
  - legacy：`src/modules/io/` + `src/modules/fastq/`
  - 现行 API：`include/fqtools/io/fastq_io.h` + Reader/Writer

### 变更
- 删除 legacy 代码：`src/modules/io/`、`src/modules/fastq/`
- 删除 legacy 测试：`tests/unit/modules/fastq/`
- CMake 清理：移除 `fq_io` 与 `fq_fastq` 目标及依赖链

### 影响
- 现行 IO/FASTQ 能力以 `fqtools/io/fastq_io.h` 及其 Reader/Writer 为准。

---

## 3. 移除聚合头 i_read_processor

### 背景
- `i_read_processor.h` 仅聚合 `read_mutator_interface.h` / `read_predicate_interface.h`，不承载真实接口。

### 变更
- 删除聚合头（两处）：`include/fqtools/pipeline/processing/i_read_processor.h`、`src/pipeline/processing/i_read_processor.h`
- 改为按需 include
- 语义化重命名：`i_read_processor.cpp` → `processing_statistics.cpp`
- 更新 CMakeLists.txt 引用

### 影响
- 仅影响内部构建与 include 关系，不改变对外接口。

---

## 4. 收敛 fq_core 头文件暴露范围

### 背景
- `fq_core` 通过 `target_include_directories` 将 `src/modules/core/` 暴露给所有依赖方，与公共 API 约束冲突。

### 变更
- 调整为只公开 `${CMAKE_SOURCE_DIR}/include`，杜绝实现路径外泄。

---

## 5. 核心模块头文件 include 边界修正

### 背景
- `src/modules/core/core.h` 通过相对路径 `../error/error.h` 引用内部实现目录。

### 变更
- 改为 `#include "fqtools/error/error.h"`，统一走公共头文件入口。

---

## 验证

以上所有变更均通过：
- `./scripts/build.sh clang Release`
- `./scripts/test.sh`
- `./tests/e2e/test_cli.sh`
