# 2025-12-22 文档文件命名规范同步

## 背景
- 项目要求所有文件名遵循小写 snake/kebab 命名，以保持一致性和可移植性。
- docs 根目录仍存在全大写文件名，且多处引用尚未更新。

## 变更
- 重命名 `docs/ARCHITECTURE.md`、`BUILD.md`、`CODING_STANDARDS.md`、`GIT_GUIDELINES.md`、`USAGE.md` 为小写形式。
- 更新 `README.md` 及 `docs/README.md` 等引用，统一链接指向新文件名。
- 形成后续命名原则：保留工具链约定文件名，其余文件统一小写。

## 影响
- 仅文档与引用更新，不影响构建流程。
- 终端用户可直接使用新的文档路径。
