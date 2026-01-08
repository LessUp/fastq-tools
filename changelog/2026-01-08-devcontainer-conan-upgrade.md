# Devcontainer Conan 版本升级（2026-01-08）

## 变更
- `docker/Dockerfile.dev`
  - Conan 从 `2.14.0` 升级到 `2.24.0`，与 CI/生产镜像保持一致。

## 影响
- 修复在 Dev Container 内执行 `conan install` 时，因 `settings.compiler.version=15` 不被识别而导致的失败（GCC 15 环境）。
- 需要执行一次 “Rebuild/Reopen in Container” 使镜像变更生效。
