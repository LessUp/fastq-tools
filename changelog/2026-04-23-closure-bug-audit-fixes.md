# 收尾阶段缺陷审计修复

## 目标

修复本地已验证的收尾阶段遗留问题，优先恢复开发入口一致性并清除当前可复现的 lint 失败。

## 主要改动

- 修复 `include/fqtools/io/fastq_writer.h` 的格式问题，恢复 `./scripts/core/lint check` 通过。
- 更新 `AGENTS.md`、`QWEN.md` 中已过时的 workflow 说明，使其与当前 `.github/workflows/ci.yml` 结构一致。
- 修正文档中的失效脚本入口与基准产物路径，统一为当前仓库实际存在的路径：
  - `scripts/tools/performance/*`
  - `scripts/tools/analysis/*`
  - `scripts/tools/release/*`
  - `docs/benchmark-reports/`
- 修复迁移后工具脚本仍按旧目录层级 `source ../lib/common.sh` 的问题，并恢复相关脚本的可执行权限，确保文档中的入口可以直接运行。
- 同步更新 `scripts/README.md` 与 `scripts/ARCHITECTURE.md`，避免脚本文档继续指向已删除的旧入口。

## 影响范围

- 代码格式检查
- Agent / 助手说明文档
- 开发者文档与脚本文档
- Benchmark 与质量工具相关操作指引

## 验证方式

```bash
./scripts/core/lint check
./scripts/core/test
python3 -m mkdocs build --strict
./scripts/tools/performance/benchmark --help
./scripts/tools/analysis/coverage-report --help
./scripts/tools/release/package-release --help
```
