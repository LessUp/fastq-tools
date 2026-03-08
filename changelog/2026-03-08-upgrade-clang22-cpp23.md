# 工具链升级：Clang 21→22、C++20→C++23

日期：2026-03-08

## 变更内容

### Clang/LLVM 升级：21 → 22

将项目 Clang 版本从 21 升级至 22（最新稳定版 22.1.0，2026-02-24 发布）。

### C++ 标准升级：C++20 → C++23

统一项目 C++ 标准为 C++23，与 fq-compressor 项目保持一致。

### 统一工具链版本

| 组件 | 旧版本 | 新版本 |
|------|--------|--------|
| **Clang/LLVM** | 21 | 22 |
| **C++ 标准** | C++20 | C++23 |
| **GCC** | 15.2 | 15.2（不变） |
| **CMake** | 4.0.2 | 4.0.2（不变） |
| **Conan** | 2.24.0 | 2.24.0（不变） |

## 受影响文件

### 构建系统
- `CMakeLists.txt` — `CMAKE_CXX_STANDARD 20` → `23`
- `CMakePresets.json` — `CMAKE_CXX_STANDARD "20"` → `"23"`，Clang 描述更新
- `config/conan/profile-clang` — `cppstd=20` → `23`，`version=20` → `22`
- `config/conan/profile-gcc` — `cppstd=20` → `23`

### Docker
- `docker/Dockerfile.dev` — Clang 21 → 22（所有包名和 alternatives）

### CI/CD
- `.github/workflows/ci.yml` — `LLVM_VERSION: "20"` → `"22"`
- `.github/workflows/quality.yml` — `LLVM_VERSION: "21"` → `"22"`

### 脚本
- `scripts/ci/release-build.sh` — Clang 21 → 22
- `scripts/tools/install-llvm` — `CURRENT_LLVM_STABLE=20` → `22`
- `scripts/core/lint` — `--std=c++20` → `--std=c++23`
- `scripts/lib/gcov-wrapper` — `llvm-cov-21` → `llvm-cov-22`

### 文档
- `AGENTS.md`, `CLAUDE.md` — 编译器版本和 C++ 标准
- `README.md`, `CONTRIBUTING.md` — 环境要求
- `.windsurf/rules.md` — 代码风格标准
- `docs/decisions/toolchain-policy.md` — 版本规范全面更新
- `docs/dev/build.md`, `docs/dev/architecture.md`, `docs/dev/coding-standards.md`
- `docs/guide/getting-started.md`, `docs/specs/steering/tech.md`
- `docs/api/core.md`, `config/README.md`
- `.devcontainer/README.md`, `scripts/tools/README.md`

## 动机

- Clang 22.1.0 是 2026-02-24 发布的最新稳定版，旧版 21 已不再是最新
- C++23 升级与 fq-compressor 项目统一标准，两项目共享一致的工具链选型
- GCC 15.2 + Clang 22 对 C++23 的支持已充分成熟
