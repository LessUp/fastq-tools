# 2025-10-20 仓库体检、开源就绪与目录重构

> 本文件合并自 3 个同日同主题的独立 changelog 条目。

## 背景

首轮仓库体检，遵循 KISS 原则，面向开源发布进行最小必要的修复与重组。

## 变更

### 1. 仓库体检与初始修复

- `README.md`: 修正编译器要求、用户指南链接
- `scripts/build.sh`: Conan 强制 `compiler.cppstd=20`，修复 ASAN/TSAN 标志拼接
- `scripts/test.sh`: 统一构建目录命名 `build-<compiler>-<build_type>`
- `scripts/coverage.sh`: 统一覆盖率构建，修复 Python here-doc 参数
- `src/CMakeLists.txt`: `mimalloc-static` → `mimalloc::mimalloc`
- `tests/utils/CMakeLists.txt`: 统一 GTest 目标名
- `config/dependencies/conanfile.py`: 仅 Clang + libc++ 时追加 `-stdlib=libc++`

### 2. 目录精简与结构重构

- 内部文档迁移至 `docs/internal/`
- 第三方头文件规范化：`src/external/` → `third_party/gzstream/`
- 移除 `src/CMakeLists.txt` 中硬编码的 Conan 缓存路径
- 工具链探测放宽（Conan/vcpkg/系统包三级 fallback）
- `.gitignore` 调整，CI 新增 coverage 任务
- 开发脚本从 `tools/development/` 迁移到 `scripts/`

### 3. 开源就绪补充

- 新增：`CONTRIBUTING.md`、`CODE_OF_CONDUCT.md`、`SECURITY.md`、`.github/CODEOWNERS`
- 新增：`third_party/README.md`
- `scripts/build.sh` 自动启用 ccache
- CI 优化：Conan/ccache 缓存配置

## 影响

- 不影响功能与构建产物
- 文档与脚本路径更新，CI 增加缓存加速
