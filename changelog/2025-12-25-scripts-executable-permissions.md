# 2025-12-25 修复脚本可执行权限

## 背景
- 项目文档与 workflows 中大量使用 `./scripts/*.sh` 方式执行脚本。
- 本次重命名与同步更新后，发现部分脚本文件缺少可执行权限，导致 `Permission denied`。

## 变更
- 为 `scripts/` 下的 `*.sh` 脚本补齐可执行权限（`chmod +x`），确保以下调用方式可用：
  - `./scripts/build.sh ...`
  - `./scripts/test.sh ...`
  - 以及其它 `./scripts/*.sh` 入口脚本

## 影响
- 仅修改文件 mode（可执行位），不改变脚本内容与业务逻辑。
