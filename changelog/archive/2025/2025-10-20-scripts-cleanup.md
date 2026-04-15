# 2025-10-20 脚本精简与合并

> 本文件合并自 2 个同日同主题的独立 changelog 条目。

## 背景

脚本目录存在大量冗余：多个编译器专用脚本（build-clang.sh、build-gcc.sh 等）功能重叠，一次性辅助脚本未清理。

## 变更

### 删除冗余脚本

- `scripts/build-clang.sh`、`build-gcc.sh`、`build-clang-simple.sh`、`sanitize.sh`、`dev.sh` — 功能已由 `build.sh` 统一覆盖
- `scripts/validators/code_quality.py` — 未被 CI/构建引用
- `scripts/verify_refactor.py`、`update_includes.py`、`normalize_comments.py`、`optimize_structure.py` — 一次性工具，与当前结构不符
- `scripts/generators/create_module.py` — 生成路径与实际目录不一致
- `examples/advanced/`（仅含误导性 README）

### 保留脚本（统一入口）

build.sh、coverage.sh、lint.sh、format.sh、test.sh、package.sh、install_dependencies.sh、dev-container-setup.sh

### 文档同步

- `docs/dev/build_system.md`: 移除对已删除脚本的引用

## 影响

- 不影响 CI 与构建
- 开发者仅需记忆统一入口脚本
