# 工具链版本规范

> 本文档定义 FastQTools 项目所有编译器、构建工具、Docker 基础镜像的版本选型规范与升级策略。
> 所有 Dockerfile、CI 配置、Conan profile 和文档必须与本规范保持一致。

---

## 版本总览

> **统一策略**：开发、CI、生产环境使用同一套编译器版本，消除环境差异导致的问题。

| 组件 | 统一版本 | 最低兼容 |
|------|---------|---------|
| **GCC** | 15.x | 11.0 |
| **Clang/LLVM** | 21 | 12.0 |
| **CMake** | 4.x（Docker 内） | 3.28 |
| **Conan** | 2.24.0 | 2.0 |
| **Ninja** | 系统最新 | 1.10 |
| **C++ 标准** | C++20 | C++20 |

---

## 1. 编译器版本规范

### 1.1 GCC

| 层级 | 版本 | 说明 |
|------|------|------|
| **统一版本** | GCC 15.x | 开发、CI、生产全部使用，最新优化器和 C++20/23 支持 |
| **兼容下限** | GCC 11.0 | CMake 编译器检查，保证源码在较旧环境中可编译 |
| **CI 兼容性检查** | GCC 14.x | `allow-failure`，作为向下兼容信号 |

**选型理由：**

- 项目通过 Docker 分发预编译二进制，用户无需自行编译，编译器版本对用户透明
- 统一版本消除"开发能编译、生产编译失败"的环境差异问题
- GCC 15.2 是 bugfix 版本，C++20 支持已充分成熟
- 维护一套配置比维护两套的认知负担和出错概率显著更低
- 实际上 Dockerfile.prod 和 Dockerfile.deploy 已经在使用 GCC 15.2，统一是让规范与现实一致

**升级策略：**

- GCC 新稳定版发布后 **1 个月内** 评估升级
- 兼容下限：每 **2 年** 评估一次，跟随主流 LTS 发行版

### 1.2 Clang/LLVM

| 层级 | 版本 | 说明 |
|------|------|------|
| **统一版本** | Clang 21 | 开发、CI、Sanitizer 全部使用，最新检查规则和工具链 |
| **兼容下限** | Clang 12.0 | CMake 编译器检查，保证源码在较旧环境中可编译 |
| **CI 兼容性检查** | Clang 19 | `allow-failure`，作为向下兼容信号 |

**选型理由：**

- Clang 的核心价值在于 Sanitizer（ASan/TSan/UBSan）和静态分析工具链
- 统一到 Clang 21 确保 Sanitizer 检测结果与开发环境一致
- clang-tidy / clang-format 使用相同版本，避免规则差异

**已知风险：**

- Clang 21 通过 `llvm.sh` 安装的是 apt.llvm.org 的 nightly 分支
- 不同时间 `docker build` 可能拉到不同 commit，影响构建可复现性
- 缓解措施：CI 中缓存 Docker layer，或在 Dockerfile 中固定 apt snapshot 日期

**升级策略：**

- 跟随 LLVM trunk，无需等待稳定版
- 兼容下限：与 GCC 同步，每 2 年评估

---

## 2. Docker 镜像规范

### 2.1 基础镜像选型

| 用途 | 镜像 | 说明 |
|------|------|------|
| **构建阶段（统一）** | `gcc:15.x-bookworm` | 开发、生产、部署全部使用同一基础镜像 |
| **运行时阶段** | `debian:bookworm-slim` | 最小化体积，安全面小 |

**运行时镜像选型理由（`debian:bookworm-slim`）：**

- 项目是 C++ CLI 工具，用户拿到的是静态链接的单一二进制
- 对基础镜像的运行时依赖极少，slim 变体足够
- 体积 ~52MB，比 `ubuntu:24.04`（~78MB）小 1/3
- Debian Bookworm 支持到 2028 年，生命周期充足
- C++ 社区普遍使用 Debian 作为 Docker 基础镜像

**不选 Ubuntu 的理由：**

- 体积更大，但对本项目无额外收益
- Ubuntu 24.04 本身基于 Debian Bookworm，二进制兼容性完全一致
- 如面向生信社区分发，可另建 Ubuntu 变体镜像

**不选 Alpine 的理由：**

- Alpine 使用 musl libc，与 glibc 存在 ABI 不兼容
- TBB、libdeflate 等依赖在 musl 上可能出问题或性能下降

### 2.2 镜像 Tag 策略

```
fastqtools:latest          ← GCC 15 构建，debian:bookworm-slim 运行时（推荐）
fastqtools:<version>       ← 版本锁定，如 fastqtools:3.1.0
```

### 2.3 镜像版本锁定规则

- **必须**使用带版本号的镜像 tag（如 `gcc:15.2-bookworm`），禁止使用 `gcc:latest`
- **必须**在 Dockerfile 注释中注明选型理由和上次评估日期
- **运行时镜像**随 Debian 大版本升级（Bookworm → Trixie），预计 2025-2026 年评估

---

## 3. 构建工具版本规范

### 3.1 CMake

| 层级 | 版本 | 说明 |
|------|------|------|
| **开发/CI 实际使用** | 4.x（最新） | Docker 内手动安装 |
| **最低要求** | 3.28 | `cmake_minimum_required(VERSION 3.28)` |

**选 3.28 作为最低版本的理由：**

- CMake 3.28 是 Ubuntu 24.04 LTS 自带版本，是当前生态的自然分界线
- 3.28 支持 `CMAKE_LINKER_TYPE`、改进的 preset 特性
- 低于 3.28 的用户在 2026 年极为罕见

### 3.2 Conan

| 层级 | 版本 | 说明 |
|------|------|------|
| **锁定版本** | 2.24.0 | Dockerfile 中 `pip install conan==2.24.0` |
| **最低要求** | 2.0 | Conan 2.x API |

**升级策略：** Conan 新版本发布后评估，确认不破坏现有 profile 后升级。

### 3.3 Ninja

- 使用系统包管理器提供的版本，不锁定特定版本
- 最低要求：1.10（支持 CMake 生成的 build.ninja 格式）

---

## 4. C++ 标准

| 项目 | 值 | 说明 |
|------|---|------|
| **当前标准** | C++20 | 所有代码必须符合 C++20 |
| **未来规划** | C++23 | 待 GCC 15 + Clang 21 的 C++23 支持充分稳定后评估迁移 |

**C++23 迁移条件（全部满足后启动）：**

1. GCC 15 / Clang 21 的 C++23 特性覆盖达到可用水平
2. Conan center 的主要依赖包支持 C++23 构建
3. 项目有明确需要 C++23 特性的场景（如 `std::expected`、`std::print`）

---

## 5. CI 编译器矩阵

```yaml
# CI 矩阵配置
strategy:
  matrix:
    include:
      # ===== 主力测试（必须通过） =====
      - compiler: gcc
        version: "15"
        preset: gcc-release

      - compiler: clang
        version: "21"
        preset: clang-release

      # ===== Sanitizer 检测（必须通过） =====
      - compiler: clang
        version: "21"
        preset: clang-asan

      - compiler: clang
        version: "21"
        preset: clang-tsan

      # ===== 兼容性检查（允许失败） =====
      - compiler: gcc
        version: "14"
        preset: gcc-release
        allow-failure: true

      - compiler: clang
        version: "19"
        preset: clang-release
        allow-failure: true
```

---

## 6. 版本一致性检查清单

以下文件必须与本规范保持一致：

| 文件 | 需要一致的内容 |
|------|--------------|
| `AGENTS.md` / `CLAUDE.md` | 编译器版本、C++ 标准描述 |
| `config/conan/profile-gcc` | `compiler.version` |
| `config/conan/profile-clang` | `compiler.version` |
| `docker/Dockerfile.dev` | GCC 和 Clang 的安装版本 |
| `docker/Dockerfile.prod` | 构建阶段基础镜像版本 |
| `docker/Dockerfile.deploy` | 构建阶段基础镜像版本 |
| `CMakeLists.txt` | `cmake_minimum_required`、编译器版本检查 |
| `CMakePresets.json` | `cmakeMinimumRequired` |
| `docs/dev/build.md` | 系统要求中的编译器和工具版本 |

---

## 7. 升级评审流程

1. 每 **6 个月**（1 月 / 7 月）进行一次工具链版本评审
2. 评审内容：编译器新版本、Docker 基础镜像安全更新、Conan 兼容性
3. 评审结果记录在 `changelog/` 目录中
4. 版本变更必须同步更新本文档和上述所有关联文件

---

*最后更新：2026-03-08*
*下次评审：2026-07-01*
