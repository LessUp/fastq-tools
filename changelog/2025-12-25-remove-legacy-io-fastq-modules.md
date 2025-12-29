# 2025-12-25 删除 legacy io/fastq 模块（全面迁移到 fqtools/io/fastq_io.h）

## 背景
- 仓库同时存在两套 FASTQ/IO 相关实现：
  - legacy：`src/modules/io/` + `src/modules/fastq/`
  - 现行公共 API：`include/fqtools/io/fastq_io.h` + `include/fqtools/io/fastq_reader.h` + `include/fqtools/io/fastq_writer.h`
- 继续保留 legacy 目录会造成结构语义混乱，并增加维护成本。

## 变更
- 删除 legacy 代码：
  - `src/modules/io/`
  - `src/modules/fastq/`
- 删除 legacy FastQ 模块的单元测试目录：
  - `tests/unit/modules/fastq/`
- CMake 清理：
  - 移除目标 `fq_io` 与 `fq_fastq`
  - 移除相关 `target_link_libraries(...)` 依赖链

## 影响
- 现行 IO/FASTQ 能力以 `fqtools/io/fastq_io.h` 及其 Reader/Writer 为准。
- legacy 的 `FqRecord` / `FileInferrer` 等类型与对应测试不再保留。
