# AGENTS.md — FastQTools Agent Guide
本文件提供给会在本仓库中自动读写代码的 Agent 使用。
目标是让 Agent 快速找到正确命令、遵守既有风格，并以最小改动完成任务。

## 规则文件状态
- 当前仓库未发现 `.cursor/rules/`、`.cursorrules` 或 `.github/copilot-instructions.md`。
- 因此，根目录 `AGENTS.md` 就是本仓库的主 Agent 规则文件。

## 项目快照
- 项目名：`FastQTools`
- 语言：C++23
- 构建系统：CMake 3.28+、Ninja、Conan 2.x
- 推荐开发编译器：Clang 21 + libc++
- 备用编译器：GCC 15
- 主要依赖：`cxxopts`、`spdlog`、`fmt`、`onetbb`、`nlohmann_json`、`libdeflate`、`GTest`
- 主要运行环境是 Linux / Docker；Windows 主要用于编辑。

## 关键目录
- `include/fqtools/`：稳定公共 API；改动前先确认是否真的需要。
- `src/`：实现代码；多数功能修改应落在这里。
- `tests/unit/`：GTest 单元测试，按模块镜像源码结构。
- `tests/integration/`：跨模块集成测试。
- `tests/e2e/`：CLI 端到端测试，已通过 CTest 统一接入。
- `scripts/core/`：优先使用这里的 `build`、`test`、`lint`。
- `changelog/`：每次修改都要新增变更记录。

## 首选工作流
1. 先搜索并阅读相关实现和测试，不要猜接口。
2. 优先做最小正确改动，不要顺手重构无关代码。
3. 修改行为时同步补充或更新测试。
4. 改完后运行最小必要验证，并新增 `changelog/YYYY-MM-DD-<slug>.md`。

## 构建命令
推荐优先使用脚本；脚本会自动处理默认构建目录和 Conan toolchain。
```bash
./scripts/core/build
./scripts/core/build --dev
./scripts/core/build --compiler gcc --type Debug
./scripts/core/build --sanitizer asan
./scripts/core/build --sanitizer tsan
./scripts/core/build --coverage
./scripts/core/build --preset clang-debug
conan install config/dependencies/ --build=missing -of=build/clang-debug -s build_type=Debug
cmake --preset clang-debug
cmake --build --preset clang-debug
```
- 默认脚本构建目录遵循 `build/<compiler>-<type-lower>`，例如 `build/clang-debug`。
- 常用 presets：`gcc-debug`、`gcc-release`、`clang-debug`、`clang-release`、`clang-asan`、`clang-tsan`、`coverage`。

## 测试命令
```bash
./scripts/core/test
./scripts/core/test --unit
./scripts/core/test --integration
./scripts/core/test --e2e
./scripts/core/test --verbose
./scripts/core/test --repeat 5
./scripts/core/test --valgrind
./scripts/core/test --coverage
```
单个 CTest 测试目标：
```bash
./scripts/core/test --unit --filter '^test_io$'
ctest --test-dir build/clang-debug -R '^test_integration_pipeline$' --output-on-failure
ctest --test-dir build/clang-debug -R '^e2e_shell_cli$' --output-on-failure
```
- `./scripts/core/test --filter ...` 会传给 `ctest -R`，匹配的是 CTest 名称，不是单个 GTest case。
单个 GTest case：
```bash
cmake --build --preset clang-debug --target test_common
./build/clang-debug/tests/unit/test_common --gtest_filter=TimerTest.BasicTiming
ctest --test-dir build/clang-debug -N
./build/clang-debug/tests/unit/test_common --gtest_list_tests
```

## Lint / Format / 静态分析
```bash
./scripts/core/lint check
./scripts/core/lint format
./scripts/core/lint tidy -b build/clang-debug
./scripts/core/lint tidy-fix -b build/clang-debug
./scripts/core/lint all -b build/clang-debug
```
- 如果你修改了 C++ 源码，至少运行 `./scripts/core/lint format` 和相关测试目标。

## 代码风格
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

## 命名约定
- 类 / 结构体：`PascalCase`
- 函数 / 方法：`camelCase`
- 普通变量 / 参数：`camelCase`
- 私有 / 受保护成员：`camelCase_`
- 常量 / `constexpr` / 静态常量：`kCamelCase`
- 枚举值：`PascalCase`
- 命名空间：`lower_case`
- 测试文件：`test_<module>.cpp`
- 测试目标：如 `test_common`、`test_io`、`test_statistics`
- 这些规则由 `.clang-tidy` 的 `readability-identifier-naming` 实际约束。

## 类型与接口习惯
- 公共接口定义放在 `include/fqtools/`，实现放在 `src/`。
- 查询函数、轻量 getter、状态判断函数，优先考虑加 `[[nodiscard]]`。
- 项目大量使用 `std::string_view`；修改逻辑时必须确认生命周期安全。
- 项目强调零拷贝 FASTQ 视图和批处理；不要无必要引入额外字符串复制。
- 并行处理基于 `tbb::parallel_pipeline`；不要在热点路径随意加入串行瓶颈。

## 错误处理与日志
- 统一异常基类是 `fq::error::FastQException`。
- 常见异常类型：`IOError`、`FormatError`、`ConfigurationError`。
- 优先复用已有宏：`FQ_THROW_IO_ERROR`、`FQ_THROW_FORMAT_ERROR`、`FQ_THROW_CONFIG_ERROR`。
- 不要静默吞掉异常；在 CLI 边界捕获并记录日志。
- 日志使用 `fq::logging::trace/debug/info/warn/error/critical`，采用 fmt 风格格式串。
- 除 CLI 帮助等明确面向终端用户的输出外，不要随意直接写 `std::cout`。
- 不要使用 `std::endl`；统一使用 `"\n"`。

## 注释与测试约定
- 公共 API 和不直观的逻辑使用简洁中文注释。
- 重要接口优先使用中文 Doxygen：`@brief`、`@param`、`@return`。
- 行为变更先看对应模块现有测试，再决定补充位置。
- 单元测试放在 `tests/unit/<module>/`；集成测试放在 `tests/integration/`。
- `tests/CMakeLists.txt` 中 `add_fq_test()` 会自动注册 CTest 名称、label 和 timeout。
- 若改动 CLI、安装导出或跨模块集成，至少补跑对应 integration / e2e。

## Agent 额外约束
- 不要引用仓库里已废弃的旧脚本路径；统一使用 `scripts/core/*`。
- 不要假设 build 产物已经存在；必要时先构建目标。
- 不要擅自重命名公共头文件、导出目标或 CLI 命令。
- commit message、changelog、代码注释优先使用中文。
- 提交规范采用 Conventional Commits：`feat|fix|docs|refactor|test|build|ci|chore(scope): subject`。

## 变更记录要求
- 每次修改都必须在 `changelog/` 新增一条记录。
- 文件名格式：`YYYY-MM-DD-<slug>.md`。
- 内容至少写清：目标、主要改动、影响范围、验证方式。
