# 2025-12-25 修复 CLI E2E 脚本执行权限

- `tests/e2e/test_cli.sh`：补齐可执行权限（`chmod +x`），以便 `./tests/e2e/test_cli.sh` 可直接运行。

