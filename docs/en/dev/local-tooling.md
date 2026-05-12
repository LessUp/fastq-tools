# Local LSP / MCP / Plugin Strategy

This page covers **local editor and helper-tool defaults only**. AI workflow and branch rules still live in `AGENTS.md` and `.github/copilot-instructions.md`.

## Default stack

- **Primary LSP**: use `clangd` first; do not stack a second C++ language engine for routine work.
- **Compilation database**: standardize on `build/clang-debug/compile_commands.json` from the `clang-debug` preset.
- **VS Code defaults**: the repo already pins `clang-debug` as the configure/build/test preset and copies `compile_commands.json` to the workspace root so `clangd`, Neovim, Cursor, and Windsurf can reuse it.
- **Recommended extensions stay minimal**: `vscode-clangd` and `CMake Tools`. The VS Code workspace disables the `cpptools` IntelliSense engine so it does not fight `clangd`.

## Quick start

```bash
# Generate the compilation database
./scripts/core/build --preset clang-debug

# or configure only
cmake --preset clang-debug
```

After that, the editor should read either:

- `build/clang-debug/compile_commands.json` (`.clangd` fallback path)
- or workspace-root `compile_commands.json` (copied there by VS Code CMake Tools)

## MCP boundary

- **Do not add a repo-specific MCP server by default**. This is a near-finished local C++/CMake/Conan repo, and `clangd + compile_commands + scripts/core/*` already covers most navigation, diagnostics, and patching work.
- **Use MCP only when you need off-repo state**, such as GitHub PRs/issues/Actions, external docs, or web search.
- **MCP does not replace the local toolchain**: header jumps, references, completion, and diagnostics should come from `clangd`, not from an extra search-oriented MCP layer.

## Integrations we are intentionally not adopting

- **No local code-search or filesystem MCP**: the repo already has `clangd`, `rg`, `git`, and `gh`; another MCP index adds duplicate context with little value.
- **No parallel C++ LSPs**: avoid `clangd + cpptools IntelliSense`, `clangd + ccls`, etc. They create duplicate diagnostics, conflicting flags, and noisy completions; the VS Code workspace already disables the `cpptools` IntelliSense engine.
- **No dedicated MCP for CMake or Conan**: presets, `scripts/core/build`, and `compile_commands.json` are already the authoritative path.
- **No editor-specific plugin matrix**: Cursor, Windsurf, VS Code, and Neovim only need to consume the same compilation database; the repo will not maintain a separate plugin recipe for each editor.

## Trade-offs

- The upside is a smaller setup, cleaner context, and behavior closer to CI.
- The trade-off is that remote platform data (PR status, Actions logs, web docs) still stays opt-in through `gh`, a browser, or MCP instead of being forced into one plugin layer.
