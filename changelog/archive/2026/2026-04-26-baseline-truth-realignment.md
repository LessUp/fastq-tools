# 2026-04-26 baseline 真值对齐与元数据修复

## 目标

把最容易误导后续维护者的几类“纸面事实”改回当前仓库现实：产品范围、公共 API、配置/CLI/output schema、工具链策略，以及仓库主页 URL 元数据。

## 主要改动

1. 重写 `openspec/baseline/product/fastq-processing.md`，把产品面收敛到当前真实维护范围：`stat`、`filter`、gzip 路径、公开 C++ API、最小脚本入口。
2. 重写 `openspec/baseline/api/core-api.md`，按 `include/fqtools/` 中的真实公共头文件重新定义 I/O、processing、statistics、config 接口。
3. 重写 `openspec/baseline/schemas/schema.md`，移除虚构的 JSON / bzip2 / xz 合约，改为当前真实 runtime structs、CLI 参数和 `stat` 文本输出格式。
4. 重写 `openspec/baseline/architecture/0002-toolchain-policy.md`，将“单一 Docker 统一编译器版本”的旧表述调整为当前仓库真实的 local-first policy、兼容性下限和首选 profile。
5. 统一 benchmark 主路径到 `docs/performance/benchmark-reports`，同步修正 baseline、workflow、工具脚本、开发文档和公开头文件中的旧路径。
6. 同步更新 `docs/dev/build*.md` 与 `docs/guide/getting-started*.md` 的工具链与压缩支持描述。
7. 修复活跃文件中的仓库 URL 大小写漂移，并收敛 `CMakePresets.json` 中误导性的固定版本描述。

## 影响范围

- `openspec/baseline/product/fastq-processing.md`
- `openspec/baseline/api/core-api.md`
- `openspec/baseline/schemas/schema.md`
- `openspec/baseline/architecture/0002-toolchain-policy.md`
- `docs/dev/build.md`
- `docs/dev/build.en.md`
- `docs/dev/benchmark-guide.md`
- `docs/dev/benchmark-guide.en.md`
- `docs/guide/getting-started.md`
- `docs/guide/getting-started.en.md`
- `.github/workflows/benchmark.yml`
- `tools/benchmark/CMakeLists.txt`
- `tools/benchmark/compiler_benchmark.py`
- `tools/benchmark/scripts/gen_benchmark_data.py`
- `tools/benchmark/scripts/report_generator.py`
- `tools/benchmark/scripts/badge_generator.py`
- `include/fqtools/benchmark/result_storage.h`
- `openspec/baseline/architecture/0003-benchmark-system.md`
- `docs/performance/benchmark-report.md`
- `CMakePresets.json`
- `CMakeLists.txt`
- `conanfile.py`
- `examples/basic_usage/README.md`

## 验证方式

- `git --no-pager diff --check`
- `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`

## 风险说明

- benchmark 路径、workflow 精简、GitHub about/pages 设置、AI 治理文档封板仍在后续任务中，当前这条记录只覆盖“最关键的事实来源修正”。
