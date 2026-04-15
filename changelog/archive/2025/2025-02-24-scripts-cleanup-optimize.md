# 2025-02-24 Scripts 目录清理与优化

## 背景
- `scripts/` 根目录存在大量重复的 `.sh` 文件（与 `tools/` 下的同名脚本内容完全相同）
- 多个脚本未归入 `core/`、`tools/`、`lib/` 三层架构
- 部分 tools/ 脚本仍引用已删除的旧脚本路径
- 存在冗余文档（README.new.md、.deprecated-notice、QUICKSTART.md）
- 多个 tools/ 脚本各自重复定义颜色和 log 函数，未复用 `lib/common.sh`

## 变更

### P1: 删除重复文件
- 删除 `scripts/benchmark_io.sh`（与 `tools/benchmark-io` 完全重复）
- 删除 `scripts/devcontainer_setup.sh`（与 `tools/setup-devcontainer` 完全重复）
- 删除 `scripts/docker_deploy.sh`（与 `tools/deploy` 完全重复）
- 删除 `scripts/generate-diff.sh`（与 `tools/generate-diff` 完全重复）
- 删除 `scripts/package_release.sh`（旧版，`tools/package-release` 已更新）
- 删除项目根目录 `generate-diff.sh`（第三份副本）

### P2: 迁移未归类脚本
- `scripts/benchmark` → `scripts/tools/benchmark`（性能基准测试 CLI）
- `scripts/gcov_wrapper.sh` → `scripts/lib/gcov-wrapper`（gcov/llvm-cov 包装器）
- `scripts/llvm.sh` → `scripts/tools/install-llvm`（LLVM 工具链安装）

### P3: 修复过时引用和路径 bug
- **`tools/deploy`**: `cd` 路径从 `../..` 修正为正确的两级上跳到项目根
- **`tools/deploy`**: 头部注释从 `scripts/docker_deploy.sh` 更新为 `scripts/tools/deploy`
- **`tools/setup-devcontainer`**: 输出信息中的旧脚本引用（`build.sh` 等）更新为 `core/build` 等
- **`tools/package-release`**: 头部注释路径更新
- **`tools/benchmark`**: `PROJECT_ROOT` 路径修正（从 `scripts/tools/` 需上跳两级）
- **`tools/generate-diff`**: 头部注释从 `generate-diff.sh` 更新为 `scripts/tools/generate-diff`
- **`.github/workflows/release.yml`**: `scripts/package_release.sh` → `scripts/tools/package-release`
- **`MIGRATION.md`**: 更新“其他脚本”映射表，标记为“已迁移”；更新 Q1 废弃说明
- **`DEPLOYMENT.md`**: 所有 `scripts/docker_deploy.sh` → `scripts/tools/deploy`
- **`docs/installation.md`**: 同上
- **`docs/archive/implementation-progress.md`**: `scripts/package_release.sh` → `scripts/tools/package-release`
- **`docs/dev/design.md`**: 同上
- **`.windsurf/rules.md`**: `scripts/benchmark` → `scripts/tools/benchmark`；移除 deprecated/ 引用
- **`scripts/core/README.md`**: 移除 deprecated/ 引用

### P4: 精简文档
- 删除 `scripts/README.new.md`（与实际 `core/` 结构不符的草稿）
- 删除 `scripts/.deprecated-notice`（引用的旧脚本和 deprecated/ 目录均已不存在）
- 删除 `scripts/QUICKSTART.md`（内容与 README.md 高度重复）
- 更新 `scripts/ARCHITECTURE.md`：移除 deprecated/ 引用，补充完整的 tools/ 和 lib/ 列表
- 更新 `scripts/README.md`：同步目录结构、工具列表、版本历史
- 更新 `scripts/tools/README.md`：补充 benchmark、install-llvm、coverage-report、run-fuzzer、valgrind-* 等工具文档

### P5: 工具脚本重构复用 `lib/common.sh`
- **`tools/deploy`**: 移除自定义颜色/log 函数，改为 `source lib/common.sh`
- **`tools/setup-devcontainer`**: 同上，`print_*` 全部替换为 `log_*`
- **`tools/benchmark-io`**: 添加 `source lib/common.sh`，`echo` 替换为 `log_info`
- **`tools/benchmark`**: 移除自定义 log 函数，改为 `source lib/common.sh`

## 影响
- `scripts/` 根目录不再有散落的 `.sh` 脚本，所有脚本归入 `core/`、`tools/`、`lib/` 三层
- CI workflow（release.yml）引用路径已同步更新
- 所有 tools/ 脚本统一使用 `lib/common.sh` 的日志和工具函数
- 文档体积减少，消除了 3 个冗余文件和多处过时引用
