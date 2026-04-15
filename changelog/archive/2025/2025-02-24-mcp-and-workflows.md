# 2025-02-24 配置 MCP 服务与新增 Workflows

## MCP 服务

在 Windsurf 全局 MCP 配置中新增 3 个服务：

| 服务 | 用途 |
|------|------|
| **context7** (`@upstash/context7-mcp`) | 查阅第三方库最新文档（CMake、Conan、TBB、spdlog 等） |
| **sequential-thinking** (`@modelcontextprotocol/server-sequential-thinking`) | 结构化多步推理，适用于复杂架构分析和性能优化决策 |
| **github** (`@modelcontextprotocol/server-github`) | GitHub API 集成：PR、Issues、Actions 状态查询（需配置 PAT） |

原有服务保持不变：`filesystem`、`git`、`fetch`。

## 新增 Workflows

| Workflow | 文件 | 用途 |
|----------|------|------|
| `/update-deps` | `.windsurf/workflows/update-deps.md` | 更新 Conan 依赖并验证构建，确保无回归 |
| `/new-module` | `.windsurf/workflows/new-module.md` | 创建新模块骨架（头文件+源文件+CMake+单测） |
| `/commit-and-push` | `.windsurf/workflows/commit-and-push.md` | 格式化→lint→测试→commit→push 一条龙 |
| `/quick-build` | `.windsurf/workflows/quick-build.md` | 快速增量构建+复跑失败测试，日常迭代用 |
| `/debug-crash` | `.windsurf/workflows/debug-crash.md` | GDB 调试崩溃/段错误，core dump 分析 |
| `/profile-perf` | `.windsurf/workflows/profile-perf.md` | perf/flamegraph 性能剖析，定位 CPU 热点 |
| `/review-diff` | `.windsurf/workflows/review-diff.md` | 提交前代码审查，检查潜在问题 |
| `/docker-dev` | `.windsurf/workflows/docker-dev.md` | Docker 开发环境管理（构建/启动/进入/清理） |
| `/search-codebase` | `.windsurf/workflows/search-codebase.md` | 代码库探索，按功能/模式/接口快速定位 |
| `/refactor-safe` | `.windsurf/workflows/refactor-safe.md` | 安全重构：重命名/提取/移动 + 自动化验证 |
