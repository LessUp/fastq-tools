# FastQTools

一个面向 FASTQ 文件的现代 C++ 高性能处理工具集，专注于生物信息学场景中的质控、过滤和统计分析。

## 快速开始

```bash
# 一键构建
./scripts/build.sh

# 查看帮助
./build-clang-release/FastQTools --help

# 统计分析
FastQTools stat -i input.fastq.gz -o output.stat.txt

# 过滤处理
FastQTools filter -i input.fq.gz -o filtered.fq.gz --min-quality 20 --min-length 50
```

## 核心功能

- **`stat` 命令**: FASTQ 文件统计分析，输出碱基/质量分布等统计结果
- **`filter` 命令**: FASTQ 读段过滤与剪切，支持质量、长度、N 比例等条件

## 技术特性

- **高性能**: 基于 Intel TBB 的并行流水线处理
- **现代化**: C++20 标准 + 现代 CMake 构建系统
- **模块化**: 清晰的接口设计，支持库级别集成

## 依赖清单

- **并发**: Intel oneTBB（流水线核心依赖）
- **压缩**: zlib / bzip2 / xz
- **CLI & 日志**: cxxopts、spdlog、fmt、nlohmann-json
- **包管理**: Conan（默认）

## 项目结构

```
FastQTools/
├── include/           # 公共 API 头文件
├── src/              # 源代码实现
├── config/           # 配置文件 (CMake、依赖管理、部署)
├── third_party/      # 第三方依赖源码 (gzstream等)
├── tools/            # 开发工具 (基准测试、构建、CI)
├── examples/         # 使用示例
├── scripts/          # 构建和测试脚本
├── tests/            # 单元测试
└── docs/            # 文档
```

### 配置目录 (config/)

包含所有配置文件，按用途组织：
- `cmake/` - CMake 配置文件，包括预设和工具链
- `dependencies/` - Conan 依赖管理配置
- `deployment/` - 部署配置，包括 Dockerfile

### 第三方依赖 (third_party/)

集中管理项目使用的第三方依赖源码/头文件（仅在遵守原许可证前提下）：

- **gzstream** (header-only)
  - 位置：`third_party/gzstream/include/gzstream.h`
  - 说明：用于 gzip 流式读写的头文件封装
  - 维护原则：只纳入必要且难以通过包管理器获取的最小集合

### 工具目录 (tools/)

包含开发、构建、测试和部署相关的工具：
- `benchmark/` - 性能基准测试工具
- `build/` - 构建相关工具和脚本
- `development/` - 开发工具（代码生成、质量验证、性能分析）
- `ci/` - 持续集成工具
- `deploy/` - 部署工具

### 示例目录 (examples/)

包含基本使用示例，帮助用户快速上手：
- `basic_stats.sh` - 基本统计分析
- `batch_processing.sh` - 批量处理多个文件
- `paired_end_analysis.sh` - 双端数据处理
- `custom_output.sh` - 自定义输出格式

## 构建与运行

### 环境要求
- CMake ≥ 3.20
- 支持 C++20 的编译器（GCC 11+ / Clang 19，推荐与 CI/Dev Container 保持一致）
- Intel oneTBB 运行时（并发库）
- Ninja（推荐）
- Conan（依赖管理）

### 快速构建
```bash
# 一键构建 (Clang + Release)
./scripts/build.sh

# 指定配置
./scripts/build.sh gcc Debug

# 启用 sanitizers (ASAN, USAN, TSAN)
./scripts/build.sh clang Debug --asan --usan
# 或者单独启用 ThreadSanitizer
./scripts/build.sh clang Debug --tsan
```

## 开发指南

- 代码与命名: 见 [编码规范](docs/dev/coding-standards.md)
- 提交信息: 见 [Git 提交规范](docs/dev/git-guidelines.md)
- Dev Container: 见 [Dev Container 团队规范](docs/dev/devcontainer-guidelines.md)
- 脚本说明: 见 [scripts/README.md](scripts/README.md)

## 文档

- [构建指南](docs/dev/build.md) - 系统要求和构建说明
- [使用指南](docs/user/usage.md) - 命令行用法和示例
- [架构设计](docs/dev/architecture.md) - 技术架构和设计原则
- [编码规范](docs/dev/coding-standards.md) - 代码与文件命名约定
- [Git 提交规范](docs/dev/git-guidelines.md) - 提交格式与示例
- [性能报告](docs/performance/benchmark-report.md) - 性能基准测试结果

<!-- BENCHMARK_START -->
## 性能

FastQTools 针对高吞吐量场景进行了优化。运行 `./scripts/benchmark run` 获取最新性能数据。

```bash
# 运行基准测试
./scripts/benchmark run

# 生成性能报告
./scripts/benchmark report

# 检测性能回归
./scripts/benchmark compare baseline.json current.json
```

详细性能报告请参阅 [性能文档](docs/performance/benchmark-report.md)。
<!-- BENCHMARK_END -->

## 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件

