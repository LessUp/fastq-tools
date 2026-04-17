# AGENTS.md — FastQTools AI Agent Guide

> 本文件提供给会在本仓库中自动读写代码的 AI Agent 使用。
> 目标是让 Agent 快速找到正确命令、遵守既有风格，并以最小改动完成任务。

## 规则文件状态

- 当前仓库未发现 `.cursor/rules/`、`.cursorrules` 或 `.github/copilot-instructions.md`。
- 因此，根目录 `AGENTS.md` 就是本仓库的主 Agent 规则文件。

---

# Project Philosophy: Spec-Driven Development (SDD)

本项目严格遵循**规范驱动开发（Spec-Driven Development, SDD）**范式。所有的代码实现必须以 `/specs` 目录下的规范文档为唯一事实来源（Single Source of Truth）。

## Directory Context (目录说明)

- `/specs/product/`：产品功能定义与验收标准。
- `/specs/rfc/`：技术设计文档（架构决策、实现方案）。
- `/specs/api/`：API 接口规范（机器可读与人类可读）。
- `/specs/db/`：数据模型与配置规范。
- `/specs/testing/`：测试策略与约定。

---

## AI Agent Workflow Instructions (AI 工作流指令)

当你（AI）被要求开发一个新功能、修改现有功能或修复 Bug 时，**必须严格按照以下工作流执行，不可跳过任何步骤**：

### Step 1: 审查与分析 (Review Specs)

- 在编写任何代码之前，首先阅读 `/specs` 目录下相关的产品文档、RFC 和 API 定义。
- 如果用户指令与现有 Spec 冲突，请立即停止编码，并指出冲突点，询问用户是否需要先更新 Spec。

### Step 2: 规范优先 (Spec-First Update)

- 如果这是一个新功能，或者需要改变现有的接口/数据库结构，**必须首先提议修改或创建相应的 Spec 文档**（如 API 规范或 RFC 文档）。
- 等待用户确认 Spec 的修改后，才能进入代码编写阶段。

### Step 3: 代码实现 (Implementation)

- 编写代码时，必须 100% 遵守 Spec 中的定义（包括变量命名、API 路径、数据类型、状态码等）。
- 不要在代码中擅自添加 Spec 中未定义的功能（No Gold-Plating）。

### Step 4: 测试验证 (Test against Spec)

- 根据 `/specs` 中的验收标准（Acceptance Criteria）编写单元测试和集成测试。
- 确保测试用例覆盖了 Spec 中描述的所有边界情况。

---

## Code Generation Rules

- 任何对外部暴露的 API 变更，必须同步修改 `/specs/api/core-api.md`。
- 如果遇到不确定的技术细节，请查阅 `/specs/rfc/` 下的架构约定，不要自行捏造设计模式。
- 任何修改行为都应记录在 `changelog/` 目录中。

---

## Project Snapshot

- **项目名**：`FastQTools`
- **语言**：C++23
- **构建系统**：CMake 3.28+、Ninja、Conan 2.x
- **推荐开发编译器**：Clang 21 + libc++
- **备用编译器**：GCC 15
- **主要依赖**：`cxxopts`、`spdlog`、`fmt`、`onetbb`、`nlohmann_json`、`libdeflate`、`GTest`
- **主要运行环境**：Linux / Docker；Windows 主要用于编辑。

---

## Key Directories

| 目录 | 用途 |
|------|------|
| `/specs/` | **规范文档**（产品需求、RFC、API 规范、测试策略）；实现代码的唯一事实来源 |
| `include/fqtools/` | 稳定公共 API；改动前先确认是否真的需要 |
| `src/` | 实现代码；多数功能修改应落在这里 |
| `tests/unit/` | GTest 单元测试，按模块镜像源码结构 |
| `tests/integration/` | 跨模块集成测试 |
| `tests/e2e/` | CLI 端到端测试，已通过 CTest 统一接入 |
| `scripts/core/` | 优先使用这里的 `build`、`test`、`lint` |
| `changelog/` | 每次修改都要新增变更记录 |

---

## Preferred Workflow

1. 先搜索并阅读相关实现和测试，不要猜接口。
2. 优先做最小正确改动，不要顺手重构无关代码。
3. 修改行为时同步补充或更新测试。
4. 改完后运行最小必要验证，并新增 `changelog/YYYY-MM-DD-<slug>.md`。

---

## Build Commands

推荐优先使用脚本；脚本会自动处理默认构建目录和 Conan toolchain。

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

**默认构建目录**：`build/<compiler>-<type-lower>`，例如 `build/clang-debug`。

**常用 presets**：`gcc-debug`、`gcc-release`、`clang-debug`、`clang-release`、`clang-asan`、`clang-tsan`、`coverage`。

---

## Test Commands

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

**单个 CTest 测试目标**：
```bash
ctest --test-dir build/clang-debug -R '^test_integration_pipeline$' --output-on-failure
ctest --test-dir build/clang-debug -R '^e2e_shell_cli$' --output-on-failure
```

**单个 GTest case**：
```bash
cmake --build --preset clang-debug --target test_common
./build/clang-debug/tests/unit/test_common --gtest_filter=TimerTest.BasicTiming
./build/clang-debug/tests/unit/test_common --gtest_list_tests
```

---

## Lint / Format / Static Analysis

```bash
./scripts/core/lint check                  # 检查代码格式
./scripts/core/lint format                 # 自动格式化
./scripts/core/lint tidy -b build/clang-debug      # clang-tidy 检查
./scripts/core/lint tidy-fix -b build/clang-debug  # clang-tidy 自动修复
./scripts/core/lint all -b build/clang-debug       # 完整检查
```

如果你修改了 C++ 源码，至少运行 `./scripts/core/lint format` 和相关测试目标。

---

## Code Style

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

---

## Naming Conventions

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

---

## Types & Interface Habits

- 公共接口定义放在 `include/fqtools/`，实现放在 `src/`。
- 查询函数、轻量 getter、状态判断函数，优先考虑加 `[[nodiscard]]`。
- 项目大量使用 `std::string_view`；修改逻辑时必须确认生命周期安全。
- 项目强调零拷贝 FASTQ 视图和批处理；不要无必要引入额外字符串复制。
- 并行处理基于 `tbb::parallel_pipeline`；不要在热点路径随意加入串行瓶颈。

---

## Error Handling & Logging

- 统一异常基类是 `fq::error::FastQException`。
- 常见异常类型：`IOError`、`FormatError`、`ConfigurationError`。
- 优先复用已有宏：`FQ_THROW_IO_ERROR`、`FQ_THROW_FORMAT_ERROR`、`FQ_THROW_CONFIG_ERROR`。
- 不要静默吞掉异常；在 CLI 边界捕获并记录日志。
- 日志使用 `fq::logging::trace/debug/info/warn/error/critical`，采用 fmt 风格格式串。
- 除 CLI 帮助等明确面向终端用户的输出外，不要随意直接写 `std::cout`。
- 不要使用 `std::endl`；统一使用 `"\n"`。

---

## Comments & Testing Conventions

- 公共 API 和不直观的逻辑使用简洁中文注释。
- 重要接口优先使用中文 Doxygen：`@brief`、`@param`、`@return`。
- 行为变更先看对应模块现有测试，再决定补充位置。
- 单元测试放在 `tests/unit/<module>/`；集成测试放在 `tests/integration/`。
- `tests/CMakeLists.txt` 中 `add_fq_test()` 会自动注册 CTest 名称、label 和 timeout。
- 若改动 CLI、安装导出或跨模块集成，至少补跑对应 integration / e2e。

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
- [specs/README.md](specs/README.md) — 规范文档目录索引
