# 2026-03-08 文档全面优化

## 变更类型

`docs`

## 变更内容

### 修复过时版本号

- `specs/steering/tech.md`: GCC 11+ → 15, Clang 19+ → 21, CMake 3.20+ → 3.28+
- `dev/coding-standards.md`: GCC 11+/Clang 12+ → GCC 15/Clang 21
- `dev/benchmark-guide.md`: Clang 19/GCC 11+ → Clang 21/GCC 15
- `dev/quality-tools.md`: GCC 11+/Clang 12+ → GCC 15/Clang 21
- `guide/deployment.md`: gcc:14.2-bookworm → gcc:15.2-bookworm, debian:12-slim → debian:bookworm-slim
- `decisions/project-assessment.md`: 更新所有第三方库版本号（oneTBB、spdlog、fmt、zlib-ng、libdeflate 等）

### 统一构建目录命名

将所有文档中旧式 `build-clang-release` 路径更新为新约定 `build/clang-release`：

- `guide/getting-started.md`
- `dev/quality-tools.md`
- `specs/steering/tech.md`

### 修复命名规范矛盾

- `dev/coding-standards.md`: 注释说脚本用 `snake_case`，但表格和实际均为 `kebab-case`，统一为 `kebab-case`
- `specs/steering/structure.md`: 同步修复脚本命名约定

### 消除内容重复

- `dev/benchmark-guide.md`: 删除重复的"测试指标"表格（与"关键指标"表格内容完全一致）

### 翻译与内容更新

- `specs/steering/product.md`: 英文 → 中文，更新项目状态
- `specs/steering/structure.md`: 标题和关键段落翻译为中文，更新 docs/ 目录结构
- `specs/steering/tech.md`: 标题和段落标题翻译为中文

### 清理元信息

- `dev/design.md`: 移除标题中"（清理 AI 生成任务项后）"的元信息

### 修复脚本路径引用

- `dev/migration.md`: 全文修复"新命令"路径（`./scripts/build` → `./scripts/core/build`，涉及 build/test/lint/install-deps）
- `dev/coding-standards.md`: "统一工具"列表中 `lint.sh`/`build.sh`/`test.sh` → `scripts/core/lint`/`build`/`test`
- `dev/design.md`: `scripts/test.sh -C` → `scripts/core/test --coverage`

### 修复目录引用

- `dev/architecture.md`: 实现层目录名 `src/pipeline/` → `src/processing/`，`src/modules/` → `src/io/`
- `specs/steering/structure.md`: config/ 子目录结构更新为实际目录（conan/dependencies/sanitizers/valgrind/cppcheck/coverage/iwyu）

### 修复二进制名大小写

- `guide/deployment.md`: 健康检查命令中 `fastqtools` → `FastQTools`

### 补充构建目录排除模式

- `dev/coding-standards.md`: 生成物目录排除补充 `build/`
- `dev/testing.md`: 覆盖率排除补充 `build/`

### 更新文档索引

- `docs/README.md`: 添加本次优化的更新记录
