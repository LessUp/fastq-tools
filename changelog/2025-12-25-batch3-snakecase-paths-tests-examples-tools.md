# 2025-12-25 批次3（部分）：tests/examples/tools 路径 snake_case 化

## 背景
- 按命名规范（对齐 MongoDB 风格的仓库文件命名），路径与脚本文件名统一使用 `snake_case`。
- 本批次按你的约束：**不重命名 `changelog/` 与 `docker/` 的文件名/路径**。

## 本次变更
- E2E 脚本改名
  - `tests/e2e/test-cli.sh` -> `tests/e2e/test_cli.sh`
- 示例目录与脚本改名
  - `examples/basic-usage/` -> `examples/basic_usage/`
  - `examples/basic_usage/basic-stats.sh` -> `examples/basic_usage/basic_stats.sh`
- Benchmark 脚本改名
  - `tools/benchmark/run-baseline.sh` -> `tools/benchmark/run_baseline.sh`

## 引用同步
- 更新 `README.md`、`examples/basic_usage/README.md`、Windsurf workflow 文档与相关 changelog 文本中的路径引用。

## 回归
- 串行验证：`./scripts/build.sh clang Release` -> `./scripts/test.sh` -> `./tests/e2e/test_cli.sh`。
- 注意：`git mv` 后 `tests/e2e/test_cli.sh` 可执行位可能丢失；本批次已补齐 `chmod +x tests/e2e/test_cli.sh`。
