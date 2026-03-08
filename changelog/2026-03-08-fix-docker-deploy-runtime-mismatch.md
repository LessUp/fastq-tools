# fix(docker): 修复部署镜像的 C++ 运行时版本不匹配

## 问题

`docker/Dockerfile.deploy` 使用 `gcc:15.2-bookworm` 构建二进制，但运行时阶段基于
`debian:bookworm-slim` 安装系统 `libstdc++6`。当二进制依赖较新的 `GLIBCXX_*`
符号版本时，运行镜像可能因系统 `libstdc++` 版本不足而启动失败。

仓库中的 `docker/Dockerfile.prod` 已改为静态链接 `libstdc++`/`libgcc`，但
`Dockerfile.deploy` 仍沿用动态依赖系统 `libstdc++6` 的方式，二者策略不一致。

## 修复

- 在 `docker/Dockerfile.deploy` 的 CMake 配置中添加：
  `-DCMAKE_EXE_LINKER_FLAGS="-static-libstdc++ -static-libgcc"`
- 移除运行时阶段对 `libstdc++6` 的安装依赖
- 使 `Dockerfile.deploy` 与 `Dockerfile.prod` 保持一致，避免 `GLIBCXX`
  版本不匹配问题

## 影响文件

- `docker/Dockerfile.deploy`
- `changelog/2026-03-08-fix-docker-deploy-runtime-mismatch.md`
