# 2025-12-25 文档与脚本命名统一：snake_case

## 背景
- 为了与代码文件命名风格保持一致，统一文档与脚本文件名为 `snake_case`。
- 同时将 `docs/` 按受众分层，降低入口文档维护成本，并避免链接断链。

## 变更
- 文档结构调整
  - `docs/usage.md` -> `docs/user/usage.md`
  - `docs/architecture.md` -> `docs/dev/architecture.md`
  - `docs/build.md` -> `docs/dev/build.md`
  - `docs/coding-standards.md` -> `docs/dev/coding_standards.md`
  - `docs/git-guidelines.md` -> `docs/dev/git_guidelines.md`
  - `docs/README.md`：更新索引链接，指向新路径
- 脚本命名统一
  - `scripts/install-deps.sh` -> `scripts/install_deps.sh`
  - `scripts/package-release.sh` -> `scripts/package_release.sh`
  - `scripts/devcontainer-setup.sh` -> `scripts/devcontainer_setup.sh`
  - `scripts/gcov-wrapper.sh` -> `scripts/gcov_wrapper.sh`
- 引用同步更新
  - 根 `README.md`、`docs/README.md`、`scripts/README.md`、`examples/basic_usage/README.md`
  - CI：`.github/workflows/release.yml`
  - Windsurf Workflows：`/.windsurf/workflows/*`
  - 相关脚本内部提示与覆盖率 wrapper 路径
  - 文档中的公共 API 示例与接口/类名称引用（对齐当前实现）

## 影响
- 仅涉及路径与文档/脚本引用更新，不改变核心业务逻辑。
- 若下游脚本或外部文档存在硬编码旧路径，需要同步调整。
