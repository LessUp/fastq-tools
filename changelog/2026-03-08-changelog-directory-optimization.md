# 2026-03-08 Changelog 目录优化

## 背景

`changelog/` 目录积累了 114 个文件，存在以下问题：
- 命名不一致：4 个文件使用了大写或混合大小写（`INSTALLATION_FIXES`、`Optimization-*`）
- 过度碎片化：同一天同一主题拆成大量小文件（如 2025-12-25 命名重构拆成 16 个独立文件）
- 缺少规范文档：没有 README 说明格式约定

## 变更

### 1. 修复命名不一致（4 个文件）
- `INSTALLATION_FIXES.md` → `installation-fixes.md`
- `Optimization-libdeflate.md` → `optimization-libdeflate.md`
- `Optimization-parser.md` → `optimization-parser.md`
- `Optimization-simd.md` → `optimization-simd.md`

### 2. 合并同日同主题文件（40 → 8）

| 合并后文件 | 原始文件数 | 主题 |
|-----------|-----------|------|
| `2025-12-24-doxygen-file-tag-consistency.md` | 2 | Doxygen @file 修复 |
| `2025-12-25-naming-convention-refactor.md` | 16 | 全项目命名规范重构 |
| `2025-12-25-legacy-code-cleanup.md` | 5 | 遗留代码与头文件清理 |
| `2025-12-25-paths-scripts-docs-standardization.md` | 5 | 路径/脚本/文档标准化 |
| `2026-01-12-devcontainer-improvements.md` | 5 | Devcontainer 综合改进 |
| `2026-01-15-io-and-stat-fixes.md` | 2 | IO 权限与 stat 命令修复 |
| `2026-01-17-devcontainer-claude-and-config.md` | 7 | Claude CLI 集成与配置 |
| `2026-01-23-devcontainer-proxy-and-data.md` | 3 | 代理与数据目录配置 |

### 3. 新增 README.md
- 定义命名规范（全小写 kebab-case）
- 定义内容格式模板（背景/变更/影响/回退方案）
- 明确合并原则（同日同主题应合并）

## 影响

- 文件数量：114 → 79（减少 31%）
- 所有原始内容完整保留在合并后的文件中
- 未来新增 changelog 有明确的规范可循
