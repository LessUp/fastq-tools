# CLAUDE.md — FastQTools Project Guide for Claude Code

> 本文档为 Claude Code 提供项目上下文，帮助快速理解代码库并生成高质量的代码变更。

## 项目概览

**FastQTools** 是一个现代化的 FASTQ 文件处理工具集，用于生物信息学领域的高通量测序数据质控。

- **语言**: C++23
- **构建**: CMake 3.28+ / Ninja / Conan 2.x
- **编译器**: GCC 15 + Clang 21 + libc++（开发/CI/生产统一）
- **并发**: Intel TBB (`tbb::parallel_pipeline`)
- **Docker**: `gcc:15.x-bookworm`（构建）/ `debian:bookworm-slim`（运行时）
- **工具链规范**: 详见 [`docs/decisions/toolchain-policy.md`](docs/decisions/toolchain-policy.md)
- **许可**: MIT

## 工具链版本规范

> 完整规范详见 [`docs/decisions/toolchain-policy.md`](docs/decisions/toolchain-policy.md)，以下为必须遵守的关键约束。

### 编译器版本

| 组件 | 统一版本 | 最低兼容 | CI 兼容性检查 |
|------|---------|---------|---------|
| **GCC** | 15.x | 11.0 | 14.x（allow-failure） |
| **Clang/LLVM** | 21 | 12.0 | 19（allow-failure） |
| **C++ 标准** | C++23 | C++23 | — |

### 构建工具版本

| 工具 | 锁定版本 | 最低要求 |
|------|---------|---------|
| **CMake** | 4.0.2（Docker 内） | 3.28 |
| **Conan** | 2.24.0 | 2.0 |
| **Ninja** | 系统最新 | 1.10 |

### Docker 镜像

| 用途 | 镜像 | 说明 |
|------|------|------|
| **构建阶段（统一）** | `gcc:15.2-bookworm` | 开发、生产、部署全部使用同一基础镜像 |
| **运行时** | `debian:bookworm-slim` | 最小化体积（~52MB），仅含必要运行时库 |

### 版本选型原则

- **环境统一**：开发、CI、生产使用同一套编译器版本，消除环境差异导致的问题
- **Docker 封装隔离风险**：前沿编译器的风险通过 Docker 镜像封装，用户拿到的是预编译二进制，不受编译器版本影响
- **禁止使用 `latest` tag**：所有 Dockerfile 必须使用带版本号的镜像 tag（如 `gcc:14.2-bookworm`）
- **半年评审周期**：每 6 个月（1 月/7 月）评审一次工具链版本，评审结果记录在 `changelog/`

## 目录结构

```
fastq-tools/
├── include/fqtools/          # 公共 API 头文件（对外接口）
│   ├── fq.h                  # Façade 入口，聚合所有公共接口
│   ├── io/                   # FASTQ I/O（FastqRecord, FastqBatch, Reader/Writer）
│   ├── processing/           # 处理管道接口（Pipeline, Predicate, Mutator）
│   ├── statistics/           # 统计计算接口
│   ├── common/               # 公共工具（Timer, Logo 等）
│   ├── config/               # 配置管理
│   ├── error/                # 错误处理
│   ├── logging.h             # 日志（基于 spdlog）
│   └── memory/               # 内存管理
├── src/                      # 实现源码
│   ├── cli/                  # 命令行入口与子命令
│   │   ├── main.cpp          # 程序入口
│   │   └── commands/         # stat / filter 子命令实现
│   ├── io/                   # I/O 实现（FastqReader, FastqWriter）
│   ├── processing/           # 处理管道实现、Predicate、Mutator
│   ├── statistics/           # 统计计算实现
│   ├── common/               # 公共工具实现
│   ├── config/               # 配置管理实现
│   └── error/                # 错误处理实现
├── tests/                    # 测试
│   ├── unit/                 # 单元测试（GTest）
│   ├── integration/          # 集成测试
│   ├── e2e/                  # 端到端测试（Python/Shell）
│   └── utils/                # 测试工具与 fixture
├── config/                   # 构建配置
│   ├── conan/                # Conan 编译器 profiles（profile-clang, profile-gcc）
│   ├── dependencies/         # Conan 依赖配置（conanfile.py）
│   ├── sanitizers/           # ASan/TSan/UBSan/MSan 运行时选项
│   ├── valgrind/             # Valgrind 抑制规则
│   ├── cppcheck/             # Cppcheck 静态分析配置
│   ├── coverage/             # 覆盖率阈值配置
│   └── iwyu/                 # Include-What-You-Use 映射
├── scripts/                  # 构建与工具脚本
│   ├── core/                 # 核心脚本（build, test, lint, install-deps）
│   ├── lib/                  # 脚本公共函数库
│   └── tools/                # 辅助工具脚本
├── docker/                   # Docker 配置（统一存放所有 Docker 相关文件）
│   ├── Dockerfile.dev        # 开发环境镜像
│   ├── Dockerfile.prod       # 生产构建镜像
│   ├── Dockerfile.deploy     # 部署镜像（多阶段构建）
│   └── docker-compose.yml    # 多服务编排
├── .devcontainer/            # VS Code DevContainer 配置
├── cmake/modules/            # 自定义 CMake 模块
├── tools/                    # 开发工具
│   ├── benchmark/            # 性能基准测试（Google Benchmark）
│   │   ├── *_benchmark.cpp   # C++ 基准测试源码
│   │   └── scripts/          # 分析脚本（报告/回归检测/徽章/数据生成）
│   ├── fuzz/                 # 模糊测试（LibFuzzer）
│   └── data/                 # 测试数据与生成脚本
├── docs/                     # 项目文档（含 deployment, migration 等）
├── changelog/                # 变更记录（每条独立 .md 文件）
├── CMakeLists.txt            # 根 CMake 配置
├── CMakePresets.json          # CMake 预设（gcc-debug/release, clang-debug/release, asan, tsan, coverage）
├── conanfile.py              # Conan 包定义（用于 conan create 分发）
└── .clang-format / .clang-tidy  # 代码格式与静态检查规则
```

## 构建与开发

### 开发环境

推荐使用 DevContainer（Docker 容器内开发），已预配置完整工具链：

```bash
# 宿主机准备
bash .devcontainer/scripts/host-prepare.sh

# VS Code: Ctrl+Shift+P → "Reopen in Container"
# 或手动启动
bash docker/start_devcontainer.sh
```

### 构建命令

```bash
# 统一构建脚本（推荐）
./scripts/core/build                             # 默认: Clang Release
./scripts/core/build --compiler gcc --type Debug  # GCC Debug
./scripts/core/build --dev                        # 开发模式: Debug + 详细输出
./scripts/core/build --sanitizer asan             # ASan 构建
./scripts/core/build --coverage                   # 覆盖率构建
./scripts/core/build --preset clang-release       # 使用 CMake preset

# 手动构建（等效命令）
conan install config/dependencies/ --build=missing -s build_type=Debug -of=build/clang-debug
cmake --preset clang-debug
cmake --build --preset clang-debug
```

### 测试命令

```bash
# 统一测试脚本（推荐）
./scripts/core/test                       # 运行所有测试
./scripts/core/test --filter "*timer*"    # 过滤特定测试
./scripts/core/test --coverage            # 覆盖率报告
./scripts/core/test --type unit           # 仅单元测试

# 手动测试（等效命令）
ctest --test-dir build/clang-debug --output-on-failure
```

### 代码质量

```bash
./scripts/core/lint                       # clang-format + clang-tidy 检查
./scripts/core/lint --fix                 # 自动修复
```

## 架构与设计模式

### 核心架构

- **接口-实现分离**: 公共 API 在 `include/fqtools/`，实现在 `src/`。CLI 层只依赖接口。
- **工厂模式**: `createProcessingPipeline()` / `createStatisticCalculator()` 创建实现实例。
- **命令模式**: CLI 子命令通过 `CommandInterface` 抽象基类注册和分发。
- **Pipeline 模式**: 数据处理使用 `tbb::parallel_pipeline`，支持多阶段并行。
- **零拷贝 I/O**: `FastqRecord` 使用 `string_view` 指向 `FastqBatch` 的连续内存。
- **Predicate/Mutator 链**: 可组合的过滤条件和读段修改器。

### 命名空间

| 命名空间 | 用途 |
|-----------|------|
| `fq::io` | FASTQ I/O（Reader, Writer, FastqRecord, FastqBatch） |
| `fq::processing` | 处理管道、Predicate、Mutator |
| `fq::statistic` | 统计计算 |
| `fq::cli` | 命令行接口 |
| `fq::cli::commands` | 具体子命令实现 |
| `fq::common` | 公共工具 |
| `fq::config` | 配置管理 |
| `fq::error` | 错误处理 |
| `fq::logging` | 日志 |

### 子命令

| 命令 | 说明 |
|------|------|
| `FastQTools stat -i input.fq -o output.json` | FASTQ 文件统计分析 |
| `FastQTools filter -i input.fq -o output.fq [--min-quality N] [--trim-quality N]` | 过滤与修剪 |

## 编码规范

### 命名约定（`.clang-tidy` 强制）

| 类型 | 规则 | 示例 |
|------|------|------|
| 类/结构体 | PascalCase | `FastqBatch`, `StatCommand` |
| 函数/方法 | camelCase | `validateLengths()`, `getPassRate()` |
| 变量/参数 | camelCase | `totalReads`, `inputFile` |
| 私有成员 | camelCase + `_` 后缀 | `config_`, `pipeline_` |
| 常量 | `k` 前缀 + camelCase | `kDefaultBatchSize` |
| 命名空间 | lower_case | `fq::processing` |

### 代码风格

- **格式化**: `.clang-format`（Google 风格变体，4 空格缩进，列宽 100）
- **返回类型**: 使用尾置返回类型 `auto foo() -> int`
- **换行符**: LF（`.editorconfig` 强制）
- **注释**: 中文 Doxygen（`@brief`, `@param`, `@return`）
- **错误处理**: 异常 + `ErrorContext` 上下文信息
- **日志**: `fq::logging::info/warn/error()` — 基于 spdlog/fmt
- **禁止**: `std::endl`（使用 `'\n'`），魔术数字（使用命名常量）

### 头文件

- 公共 API: `#include <fqtools/xxx/xxx.h>`
- 内部实现: `#include "xxx.h"`（相对路径）
- 头文件保护: `#pragma once`

## 依赖（Conan）

| 包 | 版本 | 用途 |
|----|------|------|
| `cxxopts` | 3.1.1 | 命令行参数解析 |
| `spdlog` | 1.17.0 | 日志（header-only 模式） |
| `fmt` | 12.1.0 | 格式化（header-only 模式） |
| `zlib-ng` | 2.3.2 | gzip 压缩/解压 |
| `nlohmann_json` | 3.11.3 | JSON 处理 |
| `onetbb` | 2022.3.0 | 并行处理（parallel_pipeline） |
| `libdeflate` | 1.25 | 高性能 deflate |
| `GTest` | (Conan) | 测试框架 |

## Git 提交规范

遵循 [Conventional Commits](https://www.conventionalcommits.org/)：

```
<type>(<scope>): <subject>

type: feat | fix | docs | style | refactor | perf | test | build | ci | chore | revert
scope: 可选，模块名（cli, io, stats, processing, docker, ci 等）
subject: 简短描述，不以句号结尾
```

## CMake 预设

| 预设 | 说明 |
|------|------|
| `gcc-debug` | GCC Debug |
| `gcc-release` | GCC Release（生产推荐） |
| `clang-debug` | Clang Debug + libc++（开发推荐） |
| `clang-release` | Clang Release + libc++ |
| `clang-asan` | ASan + UBSan |
| `clang-tsan` | TSan |
| `coverage` | GCC + gcov 覆盖率 |

## 变更记录

- 主 CHANGELOG: `CHANGELOG.md`（按版本号组织）
- 细粒度记录: `changelog/YYYY-MM-DD-<slug>.md`（每次变更独立文件）

**每次修改都必须在 `changelog/` 目录下创建对应的变更记录文件。**

## 注意事项

1. **构建环境**: 项目设计在 Linux/Docker 容器中构建运行（GCC 15 / Clang 21，开发与生产统一）。Windows 仅用于编辑代码，不直接编译。版本规范详见 `docs/decisions/toolchain-policy.md`。
2. **测试优先**: 修改核心逻辑前先检查/更新相关测试。测试在 `tests/unit/` 中按模块组织。
3. **接口稳定**: `include/fqtools/` 下的公共接口是稳定 API，修改需谨慎。实现变更放在 `src/` 中。
4. **不要猜测**: 使用搜索工具查找现有代码，不要假设函数签名或类接口。
5. **中文注释**: 代码注释、commit message 和文档使用中文。
6. **最小变更**: 优先用最小化改动解决问题，避免过度设计。
7. **changelog**: 每次修改必须记录到 `changelog/` 目录。
