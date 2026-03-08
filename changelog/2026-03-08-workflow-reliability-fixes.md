# 2026-03-08 Workflow 稳定性与发布构建修复

## 背景

本次调整聚焦 GitHub Actions workflow 与容器内发布构建脚本的稳定性问题，主要包括：

- `valgrind.yml` 仍引用旧式构建目录名 `build-gcc-debug`
- `quality.yml` 仅运行覆盖率测试，但未实际生成 `coverage_report/coverage.xml`
- `scripts/ci/release-build.sh` 仍残留 Clang 21 配置，且在 `sh` 下使用了非 POSIX 的 `local`
- 多个 workflow 与发布脚本在 APT / wget 下载阶段对瞬时网络抖动容错不足

## 变更

### 1. 发布构建脚本修复

- `scripts/ci/release-build.sh`
  - 将 Debian Clang 安装从 **Clang 21** 升级并统一到 **Clang 22**
  - 去除 `install_cmake()` 中的 `local ARCH`，保证 `/bin/sh` 兼容
  - Alpine 构建依赖中补充 `ca-certificates`
  - Alpine 安装 Conan 时移除不必要的 `--break-system-packages`
  - 为 Debian `apt-get` 与 `wget` 下载增加重试/补装参数，降低网络抖动导致的失败概率

### 2. 质量检查工作流修复

- `.github/workflows/quality.yml`
  - 为 `clang-format` / `clang-tidy` / coverage 相关依赖安装增加重试与 `--fix-missing`
  - 在 coverage job 中新增 `Generate coverage report` 步骤
  - 通过 `scripts/tools/coverage-report -b build-gcc-coverage -o coverage_report --xml` 生成 `coverage_report/coverage.xml`

### 3. Valgrind 工作流修复

- `.github/workflows/valgrind.yml`
  - 将错误的旧路径 `build-gcc-debug` 统一修正为 `build/gcc-debug`
  - 修正 Memcheck 日志上传路径
  - 修正 Helgrind 搜索测试二进制的构建目录
  - 为依赖安装与 CMake 下载增加重试配置

### 4. Benchmark / CI / Release 工作流稳健性修复

- `.github/workflows/benchmark.yml`
  - 为系统依赖安装与 CMake 下载增加重试配置
- `.github/workflows/ci.yml`
  - 为系统依赖、Clang 安装与 CMake 下载增加重试配置
- `.github/workflows/release.yml`
  - 将 `wait-on-check-action` 的 `running-workflow-name` 修正为当前 workflow 名 `Release`

## 影响

- 发布容器构建脚本与当前 **Clang 22 / CMake 4.0.2 / Conan 2.24.0** 工具链保持一致
- Valgrind workflow 不再因构建目录名不匹配而直接失效
- 质量工作流的 coverage 上传路径与实际产物一致
- Benchmark / CI / Quality / Valgrind / Release 在外网下载阶段更稳健
