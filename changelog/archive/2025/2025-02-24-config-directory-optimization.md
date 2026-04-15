# config/ 目录优化

**日期**: 2025-02-24

## 变更概述

对 `config/` 目录进行全面审查与优化，修正过时文档、同步依赖配置、消除幽灵路径引用。

## 变更明细

### 1. 重写 `config/README.md`（高优先级）

- **问题**: README 严重过时，仅列出 `cmake/`（不存在）、`dependencies/`、`deployment/`，遗漏 5 个子目录
- **修复**: 完整重写，包含目录树结构、每个子目录的文件说明、所有工具的使用示例

### 2. 同步根目录 `conanfile.py`（高优先级）

- **问题**: 根目录 `conanfile.py`（包发布配方）与 `config/dependencies/conanfile.py`（构建消费者配方）依赖不同步
- **修复**:
  - 添加缺失的 `nlohmann_json/3.11.3` 依赖（公开头文件 `include/fqtools/benchmark/types.h` 需要）
  - 添加 `configure()` 方法：fmt/spdlog header_only 模式，防止新版编译器 consteval 构建失败
  - 添加 Clang libc++ 编译器标志修复到 `generate()`

### 3. 修正 `config/deployment/Dockerfile` 注释（中优先级）

- **问题**: 注释写 "Clang 19" 但实际安装 Clang 21；写 "GCC >= 15.1" 但 Ubuntu 22.04 默认 GCC 远低于此
- **修复**: 纠正为 "Clang 21" 和准确的 GCC 描述

### 4. 修正 sanitizer options `.env` 引用（中优先级）

- **问题**: 4 个 `.options` 文件（asan/msan/tsan/ubsan）头部引用不存在的 `.env` 文件
- **修复**: 移除无效的 `source config/sanitizers/xxx.env` 引用，改用更可靠的 `grep -v` 用法

### 5. 修正 `config/sanitizers/README.md`（低优先级）

- **问题**: 提到 `tsan.supp` 和 `lsan.supp` 文件仿佛已存在，实际不存在
- **修复**: 改为"按需创建"的示例说明，附带环境变量引用方式

### 6. 创建 `config/coverage/thresholds.json`（低优先级）

- **问题**: `docs/dev/quality-tools.md` 和 `scripts/tools/coverage-report` 均引用 `config/coverage/thresholds.json`，但该文件不存在
- **修复**: 创建配置文件（line_coverage minimum 70%，function_coverage minimum 80%），同步更新 README 目录树

## 影响的文件

- `config/README.md` — 重写
- `conanfile.py` — 添加依赖 + configure() + Clang fix
- `config/deployment/Dockerfile` — 注释修正
- `config/sanitizers/asan.options` — 移除无效 .env 引用
- `config/sanitizers/msan.options` — 同上
- `config/sanitizers/tsan.options` — 同上
- `config/sanitizers/ubsan.options` — 同上
- `config/sanitizers/README.md` — 修正 suppression 文件引用
- `config/coverage/thresholds.json` — 新建
