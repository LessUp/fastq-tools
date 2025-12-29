# 2025-12-25 修正 CHANGELOG 中已移除 legacy fastq 引用

## 背景
- 根 `CHANGELOG.md` 的历史条目中存在对 `src/modules/fastq/fastq.h` 的引用。
- 该文件已在清理 legacy io/fastq 模块时删除，继续保留原路径会误导读者。

## 变更
- 将 `CHANGELOG.md` 中对 `src/modules/fastq/fastq.h` 的引用标注为 legacy/已移除，并指向现行 API `include/fqtools/io/fastq_io.h`。

## 影响
- 不影响编译与运行，仅提升文档准确性与可读性。
