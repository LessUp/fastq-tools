# Dev Container 挂载与仓库元配置补齐（2025-12-24）

## 背景
- Dev Container 使用 `docker/docker-compose.yml` 作为开发环境入口，需要保证宿主机常用配置与缓存能稳定挂载到容器内。
- 仓库需要提供基础的 Git 文本属性（EOL）与提交模板，方便 AI/人类一致产出符合规范的提交信息。

## 本次变更
- Dev Container / Docker
  - 更新 `docker/docker-compose.yml`
    - 将 `~` 形式的挂载改为 `${HOME}`（避免在某些运行环境下 `~` 不展开导致挂载失败）。
  - 更新 `docker/Dockerfile.dev`
    - 增加 `update-alternatives` 使 `clang/clang++/clangd/clang-tidy/clang-format` 默认命令可用。
    - Conan 版本对齐 CI：`2.19.0`。
    - 工具链对齐 CI：LLVM/Clang 19。
  - 更新 `docker/Dockerfile`
    - Conan 版本对齐 CI：`2.19.0`。
    - 工具链对齐 CI：LLVM/Clang 19。
  - 更新 `/.devcontainer/devcontainer.json`
    - `postCreateCommand` 设置 `commit.template` 并执行 `conan profile detect`。

- Git 与仓库元文件
  - 新增 `/.gitattributes`
    - 统一文本文件 `eol=lf`，并标记常见二进制文件为 `binary`。
  - 新增 `/.gitmessage.txt`
    - 提供 Conventional Commits 风格的提交模板。
  - 更新 `/.gitignore`
    - 仍忽略 `.vscode/*`，但允许提交 `.vscode/settings.json`。
  - 新增 `/.vscode/settings.json`
    - 启用 `git.useEditorAsCommitInput`，让 VS Code 提交时打开编辑器，从而应用 `commit.template`。

- Release 自动化
  - 新增 `/.github/workflows/release.yml`
    - 基于 tag `v*` 触发，调用 `scripts/package_release.sh` 生成 `dist/*.tar.gz` 并发布 GitHub Release。

## 影响范围
- 仅影响开发与发布流程配置，不改变核心业务逻辑。
