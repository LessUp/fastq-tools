# Devcontainer C++ 扩展与配置优化（clangd 优先）（2026-01-12）

## 背景

在 VS Code 生态（含 Cursor/Windsurf 等衍生 IDE）中，C++ 语言能力通常有两条路线：

- 使用 `clangd`（LSP，开源，跨 IDE 兼容性最好）
- 使用 `ms-vscode.cpptools`（微软 C/C++ 扩展，自研引擎与生态，功能较全但与 clangd 同装容易冲突，且在部分衍生 IDE 场景下兼容性/稳定性争议更大）

为了降低容器内扩展安装失败概率、减少语言服务冲突，并提升在 Cursor/Windsurf 下的可用性，本次将 devcontainer 的默认方案调整为 clangd 优先。

## 变更

- `.devcontainer/devcontainer.json`
- `.devcontainer/devcontainer.simple.json`

主要调整：

- 扩展列表默认改为：
  - `ms-vscode.cmake-tools`
  - `llvm-vs-code-extensions.vscode-clangd`
  - `vadimcn.vscode-lldb`
- 默认移除：
  - `ms-vscode.cpptools`
  - `ms-vscode.cpptools-extension-pack`
- CMake/clangd 相关设置补强：
  - 强制使用 CMake Presets（`cmake.useCMakePresets=always`）
  - 配置时导出 `compile_commands.json`（`CMAKE_EXPORT_COMPILE_COMMANDS=ON`）
  - 配置后将 `compile_commands.json` 复制到 `/workspace/compile_commands.json`（`cmake.copyCompileCommands`），便于 clangd 自动发现
  - 为 clangd 增加常用参数（后台索引、clang-tidy 等）

## 影响

- 默认语言服务为 clangd：跨 VS Code/Cursor/Windsurf 兼容性更好，且避免与 cpptools 的双引擎冲突。
- 默认调试器为 CodeLLDB：适配容器内已安装的 `lldb`。

## 备注/可选方案

- 若你必须使用 GDB 进行调试（而非 LLDB），可考虑单独加回 `ms-vscode.cpptools` 仅用于调试，并显式关闭其 IntelliSense 引擎以避免与 clangd 冲突。
