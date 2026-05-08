# 本地 LSP / MCP / 插件策略收敛

## 目标

为 FastQTools 建立一套偏本地、低上下文开销的编辑器策略：以 `clangd`、CMake preset 和 `compile_commands.json` 为主，避免为近完成态仓库叠加不必要的 MCP 与插件层。

## 主要改动

- 新增 `.clangd`，将本地 LSP 默认指向 `build/clang-debug` 的编译数据库。
- 扩展 `.vscode/settings.json`，让 VS Code 中的 `clangd` 直接复用 `clang-debug` preset 生成的编译数据库，并关闭 `cpptools` 的 IntelliSense 引擎以避免重复诊断。
- 新增 `.vscode/extensions.json`，只推荐 `vscode-clangd` 与 `CMake Tools` 两个必要扩展。
- 新增 `docs/dev/local-tooling.md` 与 `docs/dev/local-tooling.en.md`，明确 MCP 边界、刻意不采用的集成，以及本地优先的取舍。
- 在 `.github/copilot-instructions.md` 中补充指向该策略文档的入口，避免与现有 AI 工作流说明重复。

## 影响范围

- 编辑器本地行为：`.clangd`、`.vscode/settings.json`、`.vscode/extensions.json`
- 开发者文档：`docs/dev/local-tooling*.md`、`docs/dev/index*.md`
- Copilot 补充说明：`.github/copilot-instructions.md`

## 验证方式

```bash
python - <<'PY'
import json
from pathlib import Path

for path in [
    Path(".vscode/settings.json"),
    Path(".vscode/extensions.json"),
]:
    json.loads(path.read_text())
PY

git diff --check
```
