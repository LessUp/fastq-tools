# chore(ci): 检查并优化所有 GitHub Actions workflows

## 变更

### ci.yml — 重构精简

- **添加** `concurrency` 块，避免同一 PR 重复运行
- **升级** action 版本：`actions/cache` v3→v4、`actions/setup-python` v4→v5、`actions/upload-artifact` v3→v4
- **统一** 版本环境变量：`CONAN_VERSION`、`CMAKE_VERSION`、`LLVM_VERSION`
- **修复** YAML lint 错误（引号包裹含表达式的值、修正 matrix exclude 语法）
- **移除** 重复的 `lint` 和 `coverage` jobs（已由 quality.yml 覆盖）
- **拆分** commitlint 为独立 job，仅在 PR 时运行
- **移除** 冗余的 `master` 分支监听（仅保留 `main`）
- **所有** build_type 都运行测试（之前仅 Release 跑测试）

### quality.yml — 升级工具链

- **升级** clang-17 → clang-21，与开发环境对齐
- **添加** CMake 4.x 安装（之前缺失，使用了系统自带的旧版本）
- **固定** Conan 版本为 2.24.0（之前未锁定，`pip install conan` 可能安装不兼容版本）
- **升级** `actions/setup-python` v4→v5
- **改进** Quality Gate 输出 `$GITHUB_STEP_SUMMARY` 表格

### benchmark.yml — 补全基础设施

- **添加** `concurrency` 块
- **添加** CMake 4.x 安装（之前使用系统 cmake，版本过旧）
- **固定** Conan 版本为 2.24.0
- **修复** `conan install` 路径：`.` → `config/dependencies/`
- **添加** 缺失的 `libdeflate-dev` 系统依赖
- **升级** `actions/setup-python` v4→v5
- **修复** baseline 保存条件的括号优先级问题

### valgrind.yml — 补全基础设施

- **添加** `concurrency` 块
- **添加** CMake 4.x 安装
- **固定** Conan 版本为 2.24.0
- **升级** `actions/setup-python` 添加、`actions/cache` 保持 v4
- **改进** Summary 输出到 `$GITHUB_STEP_SUMMARY`

### release.yml — 引号修正

- **修正** YAML 值引号包裹，避免 lint 警告
- 结构无大变更，已较完善

### CODEOWNERS — 添加路径级规则

- 添加 `include/`、`CMakeLists.txt`、`config/dependencies/`、`.github/`、`docker/`、`scripts/` 等路径级所有权规则
