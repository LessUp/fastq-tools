# AGENTS.md — FastQTools AI Agent Guide

> 本文件提供给会在本仓库中自动读写代码的 AI Agent 使用。
> 目标是让 Agent 快速找到正确命令、遵守既有风格，并以最小改动完成任务。

## 规则文件状态

- 根目录 `AGENTS.md` 是仓库级 AI 治理主文件。
- `.github/copilot-instructions.md`、`.windsurf/rules.md`、`CLAUDE.md`、`QWEN.md` 仅提供工具级补充，不能与本文件冲突。

---

## 权威层级

1. `AGENTS.md`：AI Agent 的最高优先级治理文件，定义工作流、变更边界与协作规则。
2. `openspec/baseline/`：当前实现的规范事实来源；产品、架构、API、schema、测试约束都以此为准。
3. `openspec/changes/`：提案与待落地变更；仅在任务跨过 proposal 阈值或当前任务明确要求处理对应 proposal 时，才可作为执行依据。
4. `.github/copilot-instructions.md`、`.windsurf/rules.md`、`CLAUDE.md`、`QWEN.md`：面向特定 Agent 的补充速查表；若与前述内容冲突，一律以前三项为准。
5. 仓库内可执行约束（如 `.clang-format`、`.clang-tidy`、`CMakeLists.txt`、`scripts/core/*`）用于落实实现细节，不得被辅助文档覆盖。

---

## 维护期协作工作流（必须遵守）

### 0. 任务前预检

在开始任何修改前，至少执行：

```bash
git status --short --branch
```

- 当前工作树不干净时，先确认这些改动是否与本任务兼容，再决定继续、切分分支，或额外使用 worktree。
- `gh auth status`、`git fetch --prune origin`、`git worktree list`、`git branch -vv` 等检查改为**按需执行**，不再是单人项目的固定门槛。

### 1. 分支与 worktree

- 本仓库是单人项目，默认允许直接在当前分支完成改动并推送。
- 只有在需要隔离高风险实验、并行任务或大规模整理时，才额外使用 feature branch 或独立 worktree。
- 若使用分支，推荐命名：`fix/<slug>`、`docs/<slug>`、`chore/<slug>`。
- 若使用 worktree，一个 worktree 只服务一个任务；不要让多个 Agent 同时写同一 worktree。

### 2. 工具协作分工

- 一组活动改动只指定一个“主编辑 Agent”；其他工具只做审阅、验证或命令执行，不重复实现同一补丁。
- Claude / OpenCode：优先用于跨文件梳理、规范对齐、复杂改动设计、diff review 总结。
- Copilot / Codex：优先用于已定范围内的补丁实现、命令执行、测试修复、提交说明补完。
- Windsurf / Cascade：优先用于执行仓库既有工作流（build/test/lint/review-diff 等）和规则化任务。
- 切换主编辑 Agent 时，先提交、暂存，或在任务描述中留下 handoff，避免双写。

---

## 项目概述

**FastQTools** 是一个使用现代 C++23 编写的高性能 FASTQ 文件处理工具包，专为生物信息学质量控制（QC）工作流设计。

### 核心特性

- **极致性能**: 基于 Intel oneTBB 的并行流水线，高达 170 万 reads/秒
- **零拷贝 I/O**: 基于 `std::string_view` 的记录处理，最小化内存开销
- **生产就绪**: 完整的消毒剂、模糊测试和 CI/CD 验证
- **规范驱动**: 所有实现必须以 `/openspec/baseline/` 目录下的规范为唯一事实来源

### 技术栈

| 类别 | 技术 | 版本 | 用途 |
|------|------|------|------|
| 语言 | C++ | 23 | 现代特性、概念、范围 |
| 并行计算 | Intel oneTBB | 2022.3.0 | 流水线并行 |
| 构建系统 | CMake + Ninja | 3.28+ | 快速增量构建 |
| 包管理 | Conan | 2.x | 依赖管理 |
| 压缩 | libdeflate + zlib-ng | 1.25 / 2.3.2 | 高性能 gzip |
| 日志 | spdlog | 1.17.0 | 异步日志（仅头文件） |
| CLI | cxxopts | 3.1.1 | 参数解析 |
| 测试 | GoogleTest | 1.14+ | 单元/集成测试 |
| JSON | nlohmann_json | 3.11.3 | JSON 处理 |
| 格式化 | fmt | 12.1.0 | 格式化（仅头文件） |

---

## 项目结构

```
fastq-tools/
├── CMakeLists.txt              # 根 CMake 配置（项目版本 3.1.0）
├── CMakePresets.json           # 8 个构建预设
├── conanfile.py                # Conan 包发布配置
├── .clang-format               # 代码格式化配置
├── .clang-tidy                 # 静态分析配置
├── .pre-commit-config.yaml     # 预提交钩子
├── .editorconfig               # 编辑器配置
│
├── include/fqtools/            # 公共 API 头文件
│   ├── fq.h                    # 主入口头文件
│   ├── io/                     # FastqReader、FastqWriter、FastqBatch
│   ├── processing/             # 流水线、谓词、变形器接口
│   ├── statistics/             # 统计计算器接口
│   ├── common/                 # 工具类（Timer 等）
│   ├── config/                 # 配置管理
│   ├── error/                  # 异常层次结构
│   └── core/                   # 核心算法
│
├── src/                        # 实现代码
│   ├── cli/                    # CLI 入口和命令（stat、filter）
│   ├── common/                 # 通用工具
│   ├── config/                 # 配置管理实现
│   ├── error/                  # 错误处理实现
│   ├── io/                     # FASTQ I/O 实现（零拷贝）
│   ├── processing/             # 并行流水线、谓词、变形器
│   ├── statistics/             # 统计计算实现
│   └── benchmark/              # 性能基准测试
│
├── tests/                      # 测试套件
│   ├── unit/                   # 单元测试（镜像 src/ 结构）
│   ├── integration/            # 集成测试
│   ├── e2e/                    # 端到端测试
│   ├── utils/                  # 测试工具库
│   └── cmake_package_consumer/ # CMake 包消费者验证
│
├── openspec/                  # 规范文档（单一事实来源）
│   ├── baseline/              # 基础规范
│   │   ├── product/           # 产品需求
│   │   ├── architecture/      # 架构决策（RFC）
│   │   ├── api/               # API 规范
│   │   ├── schemas/           # 数据模型
│   │   └── testing/           # 测试策略
│   ├── changes/               # 变更提案
│   ├── archive/               # 归档
│   └── templates/             # 文档模板
│
├── scripts/core/               # 核心开发脚本
│   ├── build                   # 统一构建脚本
│   ├── test                    # 统一测试脚本
│   ├── lint                    # 代码质量脚本
│   └── install-deps            # 依赖安装脚本
│
├── docker/                     # Docker 配置
│   ├── Dockerfile.dev          # 开发环境镜像
│   ├── Dockerfile.prod         # 生产环境镜像
│   └── docker-compose.yml      # 编排配置
│
├── .devcontainer/              # DevContainer 配置
├── .github/workflows/          # CI/CD 工作流
├── docs/                       # MkDocs 文档
├── tools/                      # 开发工具（基准测试、模糊测试、测试数据）
└── changelog/                  # 变更记录
```

---

## 关键配置文件说明

### CMake 配置

- **CMakeLists.txt**: 定义项目、C++23 标准、编译器版本检查（GCC 11+/Clang 12+）、LTO 优化、Conan 集成
- **CMakePresets.json**: 提供 8 个配置预设：
  - `gcc-debug` / `gcc-release` / `gcc-relwithdebinfo`
  - `clang-debug` / `clang-release`（推荐开发使用）
  - `clang-asan` / `clang-tsan`（消毒剂构建）
  - `coverage`（覆盖率构建）

### Conan 配置

- **config/dependencies/conanfile.py**: 主要依赖管理文件
  - 运行时依赖：cxxopts、spdlog、fmt、zlib-ng、nlohmann_json、onetbb、libdeflate
  - 构建依赖：benchmark、gtest、cmake
  - 特殊配置：fmt/spdlog 头文件模式，hwloc 动态库

### 代码质量工具配置

- **.clang-format**: 列宽 100、4 空格缩进、左指针对齐、Attach 大括号风格
- **.clang-tidy**: 启用 bugprone-*、performance-*、modernize-*、readability-* 检查，包含命名约定强制
- **.editorconfig**: UTF-8、LF、4 空格缩进、文件末尾换行

---

## 构建命令

推荐使用统一构建脚本；脚本自动处理默认构建目录和 Conan toolchain。

```bash
# 统一构建脚本（推荐）
./scripts/core/build                             # 默认: Clang Release
./scripts/core/build --dev                        # 开发模式: Debug + 详细输出
./scripts/core/build --compiler gcc --type Debug  # GCC Debug
./scripts/core/build --sanitizer asan             # ASan 构建
./scripts/core/build --sanitizer tsan             # TSan 构建
./scripts/core/build --coverage                   # 覆盖率构建
./scripts/core/build --preset clang-debug         # 使用 CMake preset

# 手动构建（等效命令）
conan install config/dependencies/ --build=missing -of=build/clang-debug -s build_type=Debug
cmake --preset clang-debug
cmake --build --preset clang-debug
```

**默认构建目录**: `build/<compiler>-<type-lower>`，例如 `build/clang-debug`。

**常用 presets**: `gcc-debug`、`gcc-release`、`clang-debug`、`clang-release`、`clang-asan`、`clang-tsan`、`coverage`。

---

## 测试命令

```bash
# 统一测试脚本（推荐）
./scripts/core/test                       # 运行所有测试
./scripts/core/test --unit                # 仅单元测试
./scripts/core/test --integration         # 仅集成测试
./scripts/core/test --e2e                 # 仅端到端测试
./scripts/core/test --verbose             # 详细输出
./scripts/core/test --repeat 5            # 重复运行
./scripts/core/test --valgrind            # Valgrind 内存检查
./scripts/core/test --coverage            # 覆盖率报告
./scripts/core/test --filter '^test_io$'  # 过滤特定测试
```

**单个 CTest 测试目标**:
```bash
ctest --test-dir build/clang-debug -R '^test_integration_pipeline$' --output-on-failure
ctest --test-dir build/clang-debug -R '^e2e_shell_cli$' --output-on-failure
```

**单个 GTest case**:
```bash
cmake --build --preset clang-debug --target test_common
./build/clang-debug/tests/unit/test_common --gtest_filter=TimerTest.BasicTiming
./build/clang-debug/tests/unit/test_common --gtest_list_tests
```

---

## 代码质量 / Lint 命令

```bash
./scripts/core/lint check                  # 检查代码格式
./scripts/core/lint format                 # 自动格式化
./scripts/core/lint tidy -b build/clang-debug      # clang-tidy 检查
./scripts/core/lint tidy-fix -b build/clang-debug  # clang-tidy 自动修复
./scripts/core/lint all -b build/clang-debug       # 完整检查
```

如果你修改了 C++ 源码，至少运行 `./scripts/core/lint format` 和相关测试目标。

---

## 代码风格指南

### 基本规范

- 使用 C++23；不要引入编译器特定扩展，`CMAKE_CXX_EXTENSIONS` 关闭。
- `.editorconfig` 规定：UTF-8、LF、4 空格缩进、文件末尾保留换行。
- `.clang-format` 规定：列宽 100、`PointerAlignment: Left`、Attach 风格大括号。
- 不要手动对齐空格；直接运行 `./scripts/core/lint format`。
- 保持函数和控制流样式与现有代码一致；短 `if` / `loop` 不要压成单行。
- 优先使用尾置返回类型以匹配现有风格：`auto foo() -> int`。
- 公共头文件使用 `#pragma once`。
- 公共 API 头文件优先写成 `<fqtools/...>`；内部实现头文件使用引号包含。
- include 顺序交给 `clang-format`；不要手工维持特殊排序。
- 避免在头文件中写 `using namespace`。

### 命名约定

| 类型 | 规则 | 示例 |
|------|------|------|
| 类 / 结构体 | PascalCase | `FastqBatch`, `StatCommand` |
| 函数 / 方法 | camelCase | `validateLengths()`, `getPassRate()` |
| 普通变量 / 参数 | camelCase | `totalReads`, `inputFile` |
| 私有 / 受保护成员 | camelCase_ | `config_`, `pipeline_` |
| 常量 / `constexpr` / 静态常量 | kCamelCase | `kDefaultBatchSize` |
| 枚举值 | PascalCase | `CompressionType::Gzip` |
| 命名空间 | lower_case | `fq::processing` |
| 测试文件 | `test_<module>.cpp` | `test_io.cpp` |
| 测试目标 | `test_<module>` | `test_common`, `test_io` |

这些规则由 `.clang-tidy` 的 `readability-identifier-naming` 实际约束。

### 类型与接口习惯

- 公共接口定义放在 `include/fqtools/`，实现放在 `src/`。
- 查询函数、轻量 getter、状态判断函数，优先考虑加 `[[nodiscard]]`。
- 项目大量使用 `std::string_view`；修改逻辑时必须确认生命周期安全。
- 项目强调零拷贝 FASTQ 视图和批处理；不要无必要引入额外字符串复制。
- 并行处理基于 `tbb::parallel_pipeline`；不要在热点路径随意加入串行瓶颈。

### 错误处理与日志

- 统一异常基类是 `fq::error::FastQException`。
- 常见异常类型：`IOError`、`FormatError`、`ConfigurationError`。
- 优先复用已有宏：`FQ_THROW_IO_ERROR`、`FQ_THROW_FORMAT_ERROR`、`FQ_THROW_CONFIG_ERROR`。
- 不要静默吞掉异常；在 CLI 边界捕获并记录日志。
- 日志使用 `fq::logging::trace/debug/info/warn/error/critical`，采用 fmt 风格格式串。
- 除 CLI 帮助等明确面向终端用户的输出外，不要随意直接写 `std::cout`。
- 不要使用 `std::endl`；统一使用 `"\n"`。

### 注释规范

- 公共 API 和不直观的逻辑使用简洁中文注释。
- 重要接口优先使用中文 Doxygen：`@brief`、`@param`、`@return`。

---

## 测试策略

### 测试组织结构

| 目录 | 内容 | 框架 |
|------|------|------|
| `tests/unit/` | 单元测试，镜像 `src/` 结构 | GTest |
| `tests/integration/` | 跨模块集成测试 | GTest |
| `tests/e2e/` | CLI 端到端测试 | Bash + Python |
| `tests/utils/` | 测试工具库 | GTest |
| `tools/data/` | 测试数据（FASTQ 样本） | - |

### 测试命名约定

| 类型 | 约定 | 示例 |
|------|------|------|
| 测试文件名 | `test_<module>.cpp` | `test_fastq_reader.cpp` |
| 测试类名 | `<Module>Test` | `FastqReaderTest` |
| 测试用例 | `<Object>_<Scenario>_<Expected>` | `PassesReadWithHighAverageQuality` |
| 测试目标 | `test_<module>` | `test_io`, `test_processing` |

### 测试工具（tests/utils/）

- **FixtureLoader**: `loadTextFile()`, `loadLines()`, `getFixturePath()`, `createTempFastq()`, `compareFiles()`
- **TempDirectory**: RAII 临时目录，自动清理
- **PerformanceTimer**: 性能计时器
- **TestDataGenerator**: `generateFastQRecords()`, `generateRandomDNA()`, `generateRandomQuality()`, `createTempFile()`
- **FastQToolsTest**: 测试基类，提供 `tempDir_` 和 `testDataDir_`

### CTest 标签

| 标签 | 包含测试 | 默认超时 |
|------|----------|----------|
| `unit` | 所有单元测试 | 60s |
| `integration` | 集成测试、cmake 包消费者测试 | 120-180s |
| `e2e` | Bash CLI 测试、Python CLI 测试 | 180s |

---

## CI/CD 工作流

GitHub Actions 工作流位于 `.github/workflows/`：

| 工作流 | 触发条件 | 用途 |
|--------|----------|------|
| `ci.yml` | Push/PR to master | 格式检查、静态分析、构建/测试矩阵、覆盖率、Docker smoke |
| `release.yml` | 标签推送 (v*)、手动 | 多平台发布构建 |
| `benchmark.yml` | Push/PR、每周定时 | 性能基准测试与回归检测 |
| `valgrind.yml` | 每周定时、手动 | 深度内存分析 |
| `pages.yml` | 文档变更 | MkDocs 文档部署 |

### 质量门禁检查

1. 格式检查（clang-format）
2. 静态分析（clang-tidy + cppcheck）
3. ASan 测试
4. 代码覆盖率

---

## 开发环境

### Docker 支持

| 镜像 | 用途 |
|------|------|
| `Dockerfile.dev` | 完整开发环境（含 GDB、Valgrind、文档工具） |
| `Dockerfile.prod` | 最小运行时镜像 |

**使用 Docker Compose**:
```bash
cd docker
docker-compose up dev    # 开发容器
docker-compose up prod   # 生产容器
docker-compose up test   # 测试运行器
```

### DevContainer

VS Code DevContainer 配置位于 `.devcontainer/`：
- 基于 Docker Compose（dev 服务）
- 预装扩展：CMake Tools、clangd、LLDB、Docker、GitLens
- 自动配置 SSH 密钥、Git 配置挂载

---

# Project Philosophy: Spec-Driven Development (SDD)

本项目严格遵循**规范驱动开发（Spec-Driven Development, SDD）**范式。所有的代码实现必须以 `/openspec/baseline/` 目录下的规范文档为唯一事实来源（Single Source of Truth）。

## Directory Context (目录说明)

- `/openspec/baseline/product/`: 产品功能定义与验收标准。
- `/openspec/baseline/architecture/`: 技术设计文档（架构决策、实现方案）。
- `/openspec/baseline/api/`: API 接口规范（机器可读与人类可读）。
- `/openspec/baseline/schemas/`: 数据模型与配置规范。
- `/openspec/baseline/testing/`: 测试策略与约定。

---

## AI Agent Workflow Instructions (AI 工作流指令)

当你（AI）被要求开发一个新功能、修改现有功能或修复 Bug 时，遵循以下维护期工作流：

### Step 1: 审查规范 (Review Baseline)

- 在编写任何代码之前，首先阅读 `openspec/baseline/` 目录下的相关规范文档。
- 如果用户指令与现有规范冲突，请立即停止编码，并指出冲突点，询问用户是否需要先创建变更提案。

### Step 2: 判断是否需要 Proposal

- 若改动影响**行为、公共 API、schema / 文件格式、架构 / 工具链 / 发布策略、兼容性或弃用策略**，先在 `openspec/changes/<name>/` 创建或更新提案。
- 若只是 docs、测试、注释、命名、实现内整理，且**不改变 baseline 含义**，可直接进入实现流程，但提交说明必须注明相关 baseline 引用或“no baseline delta”。
- 临近维护冻结时，proposal 保持最小：写清 Why、影响的 baseline、兼容性 / 风险、验证方式即可；只有架构级改动才需要完整 design。

### Step 3: 实施改动

- 在当前工作分支执行实现；如任务风险较高，可自行增加 branch / worktree 隔离。实现范围不得超出当前 baseline 或已批准的 proposal。
- 若实现过程中发现 proposal 不再准确，先更新 proposal / tasks，再继续。
- 不添加规范外功能（No Gold-Plating）。

### Step 4: 轻量验证并整理提交

- 运行与改动直接相关的最小验证集；文档 / 规则改动至少执行 `git diff --check`。
- 在提交说明、changelog 或任务记录中写明 baseline 引用、验证结果、风险点、是否需要 archive。
- 如任务复杂或风险较高，可自行补一次人工或次级 AI 的 diff review。

### Step 5: 落地后归档

- 仅当改动已稳定落地（提交/推送完成）时，才运行 `/opsx:archive` 或手动归档到 `openspec/archive/YYYY/MM-<name>/`。
- 归档记录应回填 commit SHA（如适用），并同步 baseline。
- 尚未落地或被放弃的 proposal 继续留在 `openspec/changes/`，不要提前归档。

---

## Code Generation Rules

- 任何对外部暴露的 API 变更，必须同步修改 `openspec/baseline/api/core-api.md`。
- 如果遇到不确定的技术细节，请查阅 `openspec/baseline/architecture/` 下的架构约定，不要自行捏造设计模式。
- 任何修改行为都应记录在 `changelog/` 目录中。

---

## Agent Additional Constraints

- 不要引用仓库里已废弃的旧脚本路径；统一使用 `scripts/core/*`。
- 不要假设 build 产物已经存在；必要时先构建目标。
- 不要擅自重命名公共头文件、导出目标或 CLI 命令。
- commit message、changelog、代码注释优先使用中文。
- 提交规范采用 Conventional Commits：`feat|fix|docs|refactor|test|build|ci|chore(scope): subject`。

---

## Change Record Requirements

- 每次修改都必须在 `changelog/` 新增一条记录。
- 文件名格式：`YYYY-MM-DD-<slug>.md`。
- 内容至少写清：目标、主要改动、影响范围、验证方式。

---

## Related Documents

- [CLAUDE.md](CLAUDE.md) — Claude Code AI 助手指南
- [CONTRIBUTING.md](CONTRIBUTING.md) — 贡献指南
- [openspec/README.md](openspec/README.md) — 规范文档目录索引
- [OpenSpec GitHub](https://github.com/Fission-AI/OpenSpec) — OpenSpec 框架
