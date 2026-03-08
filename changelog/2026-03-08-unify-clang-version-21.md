# 统一 Clang/LLVM 版本为 21

## 日期

2026-03-08

## 类型

build

## 变更

将项目中所有 Clang/LLVM 版本引用统一为 **Clang 21**，消除不同文件间的版本不一致（部分写 20、部分写 22）。

### 修改文件

| 文件 | 变更内容 |
|------|---------|
| `AGENTS.md` | 编译器表格 Clang 22 → 21 |
| `CLAUDE.md` | 编译器表格 Clang 22 → 21 |
| `docs/decisions/toolchain-policy.md` | 版本总览 + CI 矩阵 Clang 22 → 21 |
| `config/conan/profile-clang` | `compiler.version` 20 → 21 |
| `CMakePresets.json` | preset 描述 Clang 20 → 21 |
| `.github/workflows/ci.yml` | `LLVM_VERSION` 20 → 21 |
| `scripts/ci/release-build.sh` | Clang 安装版本 22 → 21 |
| `scripts/tools/install-llvm` | `CURRENT_LLVM_STABLE` 22 → 21 |
| `scripts/lib/gcov-wrapper` | `llvm-cov-22` → `llvm-cov-21` |
| `README.md` | 推荐编译器版本 Clang 22 → 21 |
| `config/README.md` | profile 描述 Clang 22 → 21 |
| `docs/dev/build.md` | 系统要求 Clang 22 → 21 |
| `docs/guide/getting-started.md` | 开发依赖 Clang 22 → 21 |

## 影响

- 所有环境（开发、CI、生产）统一使用 Clang 21
- Conan profile、CMake preset、CI workflow、Docker 脚本版本一致
