---
description: FastQTools project rules for Cascade/Windsurf
---

# FastQTools: 项目规则（Windsurf/Cascade）

## 权威入口（优先使用）
- **构建**: `./scripts/core/build`
- **测试**: `./scripts/core/test`
- **代码质量**: `./scripts/core/lint`
- **依赖安装**: `./scripts/core/install-deps`
- **Benchmark 系统**: `./scripts/benchmark`（基于 `tools/benchmark` + `docs/benchmark-reports/`）

## 禁止/避免
- 旧的 `.sh` 脚本已删除，所有功能已迁移至 `scripts/core/` 和 `scripts/tools/`。

## 构建系统约定
- **CMake + Ninja**（`CMakePresets.json` 也可参考，但常用入口仍是 `scripts/core/build`）。
- 默认构建目录命名：`build-<compiler>-<type>`，例如：
  - `build-clang-debug`
  - `build-clang-release`
  - `build-clang-coverage`
- Conan：`scripts/core/build` 会在可用时自动 `conan install config/dependencies/ ...` 生成 toolchain。

## 工具链版本规范

| 组件 | 版本 |
|------|------|
| GCC | 15.x |
| Clang/LLVM | 21 |
| CMake | 4.0.2 |
| Conan | 2.24.0 |
| C++ 标准 | C++23 |

详见 [`docs/decisions/toolchain-policy.md`](docs/decisions/toolchain-policy.md)

## 代码风格（权威文档）
- 以 `docs/dev/coding-standards.md` 为准。
- C++23；clang-format/clang-tidy 配置来自 `.clang-format` / `.clang-tidy`。
- 文件/目录：`snake_case`；文档：`kebab-case`；类：`PascalCase`；函数/变量：`camelCase`。
- include 顺序：标准库 -> 第三方 -> 本项目。

## 目录约定
- 公共 API：`include/fqtools/`
- 实现：`src/`
- CLI：`src/cli/`
- 测试：`tests/`
- Benchmark：`tools/benchmark/` + `docs/benchmark-reports/`
- 文档：`docs/`（MkDocs + i18n 双语）
- 变更记录：`changelog/`

## 常用命令速查

### 构建与测试
```bash
# 默认构建（Clang Release）
./scripts/core/build

# 开发模式（Debug + 详细输出）
./scripts/core/build --dev

# GCC 构建
./scripts/core/build --compiler gcc --type Debug

# 运行测试
./scripts/core/test

# 带覆盖率的测试
./scripts/core/test --coverage
```

### 代码质量检查
```bash
# 格式检查
./scripts/core/lint format-check

# 自动格式化
./scripts/core/lint format

# clang-tidy 静态分析
./scripts/core/lint tidy -b build-clang-release

# cppcheck 静态分析
./scripts/core/lint cppcheck

# 运行所有检查
./scripts/core/lint all
```

### 分析/诊断工具
```bash
# Valgrind memcheck
./scripts/tools/valgrind-memcheck

# Valgrind cachegrind
./scripts/tools/valgrind-cachegrind

# Sanitizers (ASan/TSan/UBSan)
./scripts/core/build --sanitizer asan
./scripts/core/test -b build-clang-asan
```

### 文档
```bash
# 本地预览文档
mkdocs serve

# 构建文档站点
mkdocs build
```

## Git 提交规范

遵循 [Conventional Commits](https://www.conventionalcommits.org/)：

```
<type>(<scope>): <subject>

type: feat | fix | docs | style | refactor | perf | test | build | ci | chore | revert
scope: cli | io | stats | processing | docker | ci 等
subject: 简短描述（中文）
```

## 变更记录

每次修改必须创建 `changelog/YYYY-MM-DD-<slug>.md` 文件。

## 相关文档

- [架构设计](docs/dev/architecture.md)
- [构建指南](docs/dev/build.md)
- [编码规范](docs/dev/coding-standards.md)
- [工具链规范](docs/decisions/toolchain-policy.md)
