# 2025-12-25 I/O PIMPL 成员命名重构（MongoDB 风格）

- `include/fqtools/io/fastq_reader.h` / `src/io/fastq_reader.cpp`：`FastqReader` 的 PIMPL 成员从 `m_impl` 改为 `impl_`。
- `include/fqtools/io/fastq_writer.h` / `src/io/fastq_writer.cpp`：`FastqWriter` 的 PIMPL 成员从 `m_impl` 改为 `impl_`。

