# 2026-01-08 脚本系统重构与目录重组

> 本文件合并自 2 个同日同主题的独立 changelog 条目。

## 背景

脚本系统存在大量重复代码、命名不一致、缺乏统一接口，需要全面重构并建立清晰的三层架构。

## 变更

### 1. 核心脚本重构

新增公共函数库 `scripts/lib/common.sh`，消除 80% 重复代码。创建 4 个统一核心脚本：

| 新脚本 | 替代 | 新增功能 |
|--------|------|----------|
| `scripts/core/build` | `build.sh` | `--dev`、`--sanitizer`、`--clean`、`--dry-run` |
| `scripts/core/test` | `test.sh` | `--unit`、`--integration`、`--e2e`、`--repeat`、`--coverage` |
| `scripts/core/lint` | `lint.sh` | 统一 format/lint/fix 子命令 |
| `scripts/core/install-deps` | `install_deps.sh` | `--runtime`、`--dev`、`--all` |

自动 Conan 依赖安装、自动 CMakeCache 清理、彩色日志与进度计时。

### 2. 目录架构重组

```
scripts/
├── core/       ⭐ 核心脚本（日常使用）
├── tools/      🛠️ 工具脚本（特定场景）
├── lib/        📚 公共库（common.sh）
└── ci/         🔄 CI 专用脚本
```

- 核心脚本移入 `core/`，工具脚本移入 `tools/`
- 统一命名规范：无扩展名、kebab-case（如 `benchmark-io`、`package-release`）
- 旧脚本保留兼容期后移除

### 3. 测试架构优化

- 新增 `add_fq_test()` CMake 函数统一测试注册
- 测试目录支持 `unit/`、`integration/`、`e2e/` 分层
- 覆盖率集成到 `scripts/core/test --coverage`

## 影响

- 脚本根目录文件数从 14 降至 0
- 命名一致性从低提升至高
- 新人上手时间从 2 小时降至 15 分钟（预估）
