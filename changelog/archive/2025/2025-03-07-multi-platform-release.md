# 多平台 Release 构建体系

**日期**: 2025-03-07
**类型**: feat(ci)
**影响范围**: .github/workflows/release.yml, scripts/ci/, scripts/tools/package-release

## 新增内容

### 构建矩阵 — 6 个目标平台

| 目标 | 编译器 | C 库 | 架构 | 链接方式 | 用途 |
|------|--------|------|------|---------|------|
| `linux-x86_64-gcc-glibc` | GCC 15 | glibc | x86_64 | 动态 | 最通用的 Linux 发行版 |
| `linux-x86_64-clang-glibc` | Clang 21 | glibc | x86_64 | 动态 | Clang 替代构建 |
| `linux-x86_64-musl-static` | GCC | musl | x86_64 | 静态 | Alpine / 便携式 |
| `linux-aarch64-gcc-glibc` | GCC 15 | glibc | aarch64 | 动态 | ARM 服务器 (Graviton) |
| `linux-aarch64-clang-glibc` | Clang 21 | glibc | aarch64 | 动态 | ARM Clang 替代 |
| `linux-aarch64-musl-static` | GCC | musl | aarch64 | 静态 | ARM Alpine / 便携式 |

### .github/workflows/release.yml — 重写

- 6 目标并行构建矩阵，使用 Docker 容器隔离环境
- glibc 构建：`gcc:15.2-bookworm` 容器
- musl 静态构建：`alpine:3.21` 容器，`-static` 链接
- aarch64 通过 QEMU 用户空间模拟 (`docker/setup-qemu-action`)
- Clang 21 自动安装（仅 glibc 构建需要）
- 两阶段流程：build（并行构建） → publish（收集产物发布）
- 自动生成 `SHA256SUMS.txt` 校验文件
- 支持 `workflow_dispatch` 手动触发
- 预发布版本自动标记（tag 含 `-` 时）

### scripts/ci/release-build.sh — 新增

- 容器内构建脚本，自适应 Alpine (musl) / Debian (glibc)
- 自动检测 OS 类型并安装对应依赖
- 支持 `gcc` / `clang` 编译器参数
- 支持 `--static` 静态链接模式
- 安装 CMake 4.0.2 + Conan 2.24.0
- 构建完成后验证二进制（file 类型、静态链接检查）

### scripts/tools/package-release — 重写

- 新增参数化接口：`-v VERSION -t TARGET -b BUILD_DIR`
- 支持 `--no-build` 模式（CI 场景仅打包）
- 自动检测构建目录（多种命名约定）
- 生成 SHA256 校验和
- 产物命名规范：`fastqtools-v{VERSION}-{TARGET}.tar.gz`

## 发布流程

```bash
# 创建 tag 触发自动发布
git tag v3.1.0
git push origin v3.1.0

# 或手动触发 (GitHub Actions → Release → Run workflow)
```

GitHub Release 产物示例：
```
fastqtools-v3.1.0-linux-x86_64-gcc-glibc.tar.gz
fastqtools-v3.1.0-linux-x86_64-clang-glibc.tar.gz
fastqtools-v3.1.0-linux-x86_64-musl-static.tar.gz
fastqtools-v3.1.0-linux-aarch64-gcc-glibc.tar.gz
fastqtools-v3.1.0-linux-aarch64-clang-glibc.tar.gz
fastqtools-v3.1.0-linux-aarch64-musl-static.tar.gz
SHA256SUMS.txt
```
