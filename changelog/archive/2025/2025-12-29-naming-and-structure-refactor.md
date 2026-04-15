# Changelog: 统一项目代码风格为借鉴 MongoDB 规范

## 日期: 2025-12-29

## 变更类型
- **代码重构 (Refactoring)**
- **规范更新 (Documentation)**
- **结构优化 (Restructuring)**

## 变更详情

### 1. 命名规范统一
按照借鉴 MongoDB 风格的新规范，对全仓代码进行了系统性重命名：
- **函数与方法**: 全部由 `snake_case` 改为 `camelCase`（如 `next_batch` -> `nextBatch`, `is_open` -> `isOpen`）。
- **局部变量与参数**: 全部由 `snake_case` 改为 `camelCase`（如 `read_count` -> `readCount`）。
- **私有成员变量**: 全部由前缀下划线改为后缀下划线风格（如 `_impl` -> `impl_`, `_config` -> `config_`）。
- **常量**: 统一使用 `kConstantName` 格式。

### 2. 项目结构优化
- **扁平化 `src` 目录**: 将原 `src/modules/` 下的所有核心子模块（`common`, `config`, `core`, `error`）直接提升到 `src/` 根目录下，使项目结构更加扁平、清晰。
- **同步更新 CMake**: 相应调整了 `src/CMakeLists.txt` 以支持新的目录结构。

### 3. 文档与工具链更新
- **更新 `docs/dev/coding_standards.md`**: 明确了项目风格来源（借鉴并适配 MongoDB），完善了命名例外清单（README, CMakeLists.txt 等）。
- **强化 `.clang-tidy`**: 引入了 `readability-identifier-naming` 相关规则，通过静态检查自动化保障命名规范的执行。

### 4. 测试套件同步
- 全面重构了 `tests/unit/` 下的测试代码，确保其符合新的命名规范并能正确调用重构后的 API。

## 影响范围
- **API 变更**: 所有公共类的方法名均已变更，下游调用方需同步更新。
- **构建系统**: `src/modules` 路径已废弃，需使用新的模块路径。

## 结论
本项目现在的代码风格已完全符合借鉴 MongoDB 风格后的新项目规范要求。
