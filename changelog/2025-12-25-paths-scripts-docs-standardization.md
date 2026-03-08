# 2025-12-25 路径、脚本与文档标准化

> 本文件合并自 5 个同日同主题的独立 changelog 条目。

## 1. tests/examples/tools 路径 snake_case 化

- `tests/e2e/test-cli.sh` → `tests/e2e/test_cli.sh`
- `examples/basic-usage/` → `examples/basic_usage/`
- `examples/basic_usage/basic-stats.sh` → `examples/basic_usage/basic_stats.sh`
- `tools/benchmark/run-baseline.sh` → `tools/benchmark/run_baseline.sh`
- 引用同步更新：`README.md`、`examples/basic_usage/README.md`、Windsurf workflow 文档等

## 2. 文档与脚本命名统一：snake_case

### 文档结构调整
- `docs/usage.md` → `docs/user/usage.md`
- `docs/architecture.md` → `docs/dev/architecture.md`
- `docs/build.md` → `docs/dev/build.md`
- `docs/coding-standards.md` → `docs/dev/coding_standards.md`
- `docs/git-guidelines.md` → `docs/dev/git_guidelines.md`

### 脚本命名统一
- `scripts/install-deps.sh` → `scripts/install_deps.sh`
- `scripts/package-release.sh` → `scripts/package_release.sh`
- `scripts/devcontainer-setup.sh` → `scripts/devcontainer_setup.sh`
- `scripts/gcov-wrapper.sh` → `scripts/gcov_wrapper.sh`

### 引用同步
- 根 `README.md`、`docs/README.md`、`scripts/README.md`、`examples/basic_usage/README.md`
- CI：`.github/workflows/release.yml`
- Windsurf Workflows、相关脚本内部路径

## 3. 修复脚本可执行权限

- 为 `scripts/` 下的 `*.sh` 脚本补齐 `chmod +x`，确保 `./scripts/*.sh` 调用方式可用。

## 4. 修复 CLI E2E 脚本执行权限

- `tests/e2e/test_cli.sh`：补齐可执行权限。

## 5. 修正 CHANGELOG 中已移除 legacy fastq 引用

- 将 `CHANGELOG.md` 中对 `src/modules/fastq/fastq.h` 的引用标注为 legacy/已移除，指向现行 API `include/fqtools/io/fastq_io.h`。

## 影响

- 仅涉及路径、文档/脚本引用更新和文件权限，不改变核心业务逻辑。
