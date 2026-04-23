# FastQTools Copilot Instructions

- Primary governance lives in [`AGENTS.md`](../AGENTS.md). Use this file as a short Copilot supplement only.
- Before editing, at least run `git status --short --branch`; fetch/worktree/`gh` checks are optional when the task actually needs them.
- Default workflow for this solo repository is direct local editing plus direct push after relevant checks pass. Use an extra branch or worktree only when it genuinely helps isolate risky or parallel work.
- Proposal threshold: use `openspec/changes/` only when the task changes behavior, public API, schema/file format, architecture/toolchain, release policy, or compatibility. Docs/tests/internal cleanup can go straight to implementation with baseline references or `no baseline delta`.
- One branch should have one primary editing tool. If Claude, Codex, OpenCode, or Windsurf already owns the active changes, switch Copilot to review or validation instead of parallel edits.
- For local C++ work, prefer `clangd` + `compile_commands.json` from the `clang-debug` preset; see [`docs/dev/local-tooling.md`](../docs/dev/local-tooling.md).
- Treat MCP as opt-in for remote state (GitHub, CI, web docs). Do not add repo-specific MCP layers for local code navigation, search, or CMake/Conan control.
- Use repository entry points under `scripts/core/` for build, test, and lint work; do not invent alternate script paths.
