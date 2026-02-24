# 2025-02-24 Docker 配置优化

## 背景
- Docker 构建配置存在多个问题，包括生产构建缺依赖、共享 volume 权限冲突、构建阶段臃肿、配置不一致等。

## 变更

### Bug 修复

1. **`Dockerfile.prod` — 构建阶段补全缺失的项目依赖库**
   - 原先只安装了 `libdeflate-dev`，缺少 `zlib1g-dev`、`libbz2-dev`、`liblzma-dev`、`libtbb-dev`。
   - 可能导致生产构建失败或链接错误。

2. **`docker-compose.yml` — 修复 build 服务共享 volume 权限冲突**
   - dev/test 以 `developer`（uid 1000）写入 `conan_cache`/`ccache_cache`，build 以 `root`（uid 0）写入同一 volume。
   - 改为 build 服务使用独立的 `build_conan_cache`/`build_ccache_cache` volume。

### 优化

3. **`Dockerfile.prod` — 精简构建阶段**
   - 移除了 `vim`、`neovim`、`less`、`tree`、`grep`、`htop`、`tmux`、`ripgrep` 等交互工具。
   - 移除了 `zip`、`unzip`、`jq`、`gnupg`、`lsb-release`、`software-properties-common` 等非必要包。
   - 减少构建层大小和构建时间。

4. **`.dockerignore` — 补充排除项**
   - 添加 `docker/data/`、`docker/output/`、`docs/`、`changelog/` 等到排除列表。
   - 减少 `COPY . .` 传输的构建上下文大小。

5. **统一 `USE_CHINA_MIRROR` 默认值**
   - `Dockerfile.dev` 和 `Dockerfile.prod` 中的 `ARG USE_CHINA_MIRROR` 默认值从 1 改为 0。
   - 与 `docker-compose.yml`（`${USE_CHINA_MIRROR:-0}`）和 `.env.example` 保持一致。
   - 实际构建通过 `docker/.env` 中的 `USE_CHINA_MIRROR=1` 控制。

6. **统一代理 ARG 命名**
   - `Dockerfile.prod` 的代理参数从 `HTTP_PROXY`/`HTTPS_PROXY` 改为 `DEVCONTAINER_HTTP_PROXY`/`DEVCONTAINER_HTTPS_PROXY`，与 `Dockerfile.dev` 一致。
   - `docker-compose.yml` 的 `test` 和 `build` 服务补全了代理参数传递。
   - `docker-compose.yml` 的 `prod` 服务补全了代理参数传递。

7. **`Dockerfile.prod` — production 阶段支持中国镜像**
   - 在 `debian:bookworm-slim` production 阶段添加了 `USE_CHINA_MIRROR` 配置。

8. **`docker/data/` 和 `docker/output/` 添加 `.gitkeep`**
   - 确保 fresh clone 后目录存在，避免 `docker-compose.yml` 中 `prod` 服务的 bind mount 失败。

9. **`Dockerfile.dev` — 移除未使用的 `python3-venv`**
   - Conan 使用 `--break-system-packages` 安装，venv 从未被使用。

## 影响
- 需要重新构建 Docker 镜像以应用更改。
- 如果之前 `build` 服务使用了旧的 `conan_cache`/`ccache_cache` volume，首次使用新 volume 时会重新下载/编译依赖。
- 不影响 devcontainer 脚本（已在同日的另一次变更中修复）。
