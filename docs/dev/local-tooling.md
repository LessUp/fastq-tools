# 本地 LSP / MCP / 插件策略

本页只说明 **本地编辑器与辅助工具** 的默认策略；AI 工作流与分支协作规则仍以 `AGENTS.md` 和 `.github/copilot-instructions.md` 为准。

## 默认方案

- **LSP 主线**：优先使用 `clangd`，不再为日常开发叠加第二套 C++ 语言引擎。
- **编译数据库来源**：统一使用 `clang-debug` preset 生成的 `build/clang-debug/compile_commands.json`。
- **VS Code 默认值**：仓库已固定 `clang-debug` 作为 configure/build/test preset，并复制 `compile_commands.json` 到仓库根目录，便于 `clangd`、Neovim、Cursor、Windsurf 复用。
- **推荐插件只保留两个**：`vscode-clangd` 和 `CMake Tools`。VS Code 工作区会关闭 `cpptools` 的 IntelliSense 引擎，避免与 `clangd` 重复诊断。

## 快速使用

```bash
# 首次生成编译数据库
./scripts/core/build --preset clang-debug

# 或仅配置
cmake --preset clang-debug
```

完成后，本地编辑器应直接读取：

- `build/clang-debug/compile_commands.json`（`.clangd` 默认回退路径）
- 或仓库根目录 `compile_commands.json`（由 VS Code CMake Tools 复制）

## MCP 使用边界

- **默认不为本仓库引入 repo-specific MCP server**。这是一个近完成态的本地 C++/CMake/Conan 项目，`clangd + compile_commands + scripts/core/*` 已覆盖绝大多数导航、诊断和修复场景。
- **仅在需要仓库外状态时再用 MCP**，例如：GitHub PR / issue / Actions、外部文档、网页检索。
- **MCP 不替代本地工具链**：本地头文件跳转、引用查找、补全、诊断，优先由 `clangd` 完成，而不是额外的搜索型 MCP。

## 明确不采用的集成

- **不采用本地代码搜索 / 文件系统 MCP**：本仓库已有 `clangd`、`rg`、`git`、`gh`，额外 MCP 只会重复索引并浪费上下文。
- **不并行启用多套 C++ LSP**：例如 `clangd + cpptools IntelliSense`、`clangd + ccls`。这样容易出现重复 diagnostics、不同编译参数和补全冲突；VS Code 工作区已默认关闭 `cpptools` 的 IntelliSense 引擎。
- **不为 CMake / Conan 单独接入 MCP**：preset、`scripts/core/build` 和 `compile_commands.json` 已经是权威入口。
- **不维护编辑器专属插件矩阵**：Cursor / Windsurf / VS Code / Neovim 只要能消费 `compile_commands.json` 即可；仓库层面不再为每个编辑器写一套专用策略。

## 取舍

- 这样做的好处是：配置少、上下文干净、行为和 CI 更一致。
- 代价是：远程平台信息（PR 状态、Actions 日志、网页文档）仍需按需使用 `gh`、浏览器或 MCP，而不是全部收敛到单一插件层。
