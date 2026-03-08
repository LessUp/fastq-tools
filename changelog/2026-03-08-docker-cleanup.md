# docker 目录检查与修复

**日期**: 2026-03-08
**类型**: fix(docker), chore

## 变更内容

### Dockerfile.prod
- 移除冗余的 runtime-libs 收集和复制步骤（已通过静态链接 libstdc++/libgcc 解决）
- 移除 `ldconfig` 调用（静态链接后不再需要）
- 构建阶段移除未使用的 `libbz2-dev`/`liblzma-dev`
- 生产阶段移除未使用的 `libbz2-1.0`/`liblzma5`

### Dockerfile.deploy
- 构建阶段移除未使用的 `libbz2-dev`/`liblzma-dev`
- 运行阶段移除未使用的 `libbz2-1.0`/`liblzma5`

### Dockerfile.dev
- 移除未使用的 `libbz2-dev`/`liblzma-dev`
