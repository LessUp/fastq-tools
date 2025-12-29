# 2025-12-29 文档/示例一致性审计（修复前清单）

本文件记录本次文档修复前发现的“文档表述/示例脚本”与仓库现状的不一致项，作为修复前对照清单。

## 1. 命名规范口径与实际代码不一致

- `docs/dev/coding_standards.md`
  - **问题**：文档宣称“变量使用 `camelCase`”，但当前源码中（例如 `src/cli/main.cpp`、`src/io/fastq_reader.cpp`）存在大量 `snake_case` 局部变量/临时变量。
  - **影响**：读者按文档执行 code review 时会产生“文档与现实冲突”。

## 2. 文件/目录命名的“例外规则”未明确

- `docs/dev/coding_standards.md`
  - **问题**：文档写“代码文件/目录采用 `snake_case`”，但仓库中存在行业惯例文件名：
    - `CMakeLists.txt`
    - `README.md`
    - `Dockerfile`
    - `LICENSE`
  - **影响**：严格按文档会导致“规范无法落地/天然违规”。

## 3. examples/basic_usage/README.md 引用不存在的脚本/数据

- `examples/basic_usage/README.md`
  - **问题**：文档列出 `batch_processing.sh` / `paired_end_analysis.sh` / `custom_output.sh` / `generate_test_data.sh`，但 `examples/basic_usage/` 目录中并不存在这些文件。
  - **问题**：文档提供 `wget https://github.com/LessUp/FastQTools/raw/main/examples/data/sample.fastq.gz` 的下载路径，但仓库中并无对应 `examples/data/` 目录与文件。
  - **影响**：用户照做会直接失败。

## 4. examples/basic_usage/basic_stats.sh 与实际 CLI 不一致

- `examples/basic_usage/basic_stats.sh`
  - **问题**：脚本检查并调用 `fastqtools` 命令，但仓库内构建产物与 e2e 脚本默认指向的是 `FastQTools`（例如 `tests/e2e/test_cli.sh` 使用 `$PROJECT_ROOT/build-clang-release/FastQTools`）。
  - **问题**：脚本使用了 `-b` 作为 batch 参数，但当前 CLI 使用的是长选项 `--batch-size`（见 `src/cli/commands/stat_command.cpp`、`src/cli/commands/filter_command.cpp`）。
  - **影响**：示例脚本在默认情况下无法运行。

