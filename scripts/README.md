# 脚本约定

统一管理 `scripts/` 下的构建、测试和发布脚本，保持一致的命名与调用体验。

## 命名与风格

- 文件名：使用 `snake_case`，扩展名 `.sh`
- 入口约定：每个脚本都应提供 `--help` 或注释示例
- 安全模式：顶部包含 `#!/bin/bash` 与 `set -euo pipefail`
- 输出约定：`echo`/`printf` 带前缀（INFO/ERROR 等）方便 CI 解析

## 现有脚本

| 脚本 | 作用 | 备注 |
| --- | --- | --- |
| `build.sh` | 配置并构建项目 | 支持编译器/构建类型/asan/coverage 等选项 |
| `test.sh` | 运行 CTest 并生成覆盖率（可选） | `-c/-t/-C` 控制编译器、构建类型与覆盖率 |
| `lint.sh` | 统一的 clang-format/clang-tidy 入口 | `format`/`format-check` 子命令 |
| `install_deps.sh` | 安装系统与工具链依赖 | 适用于首次配置环境 |
| `package_release.sh` | 生成可分发产物 | 适用于发布阶段 |
| `gcov_wrapper.sh` | 为 llvm-cov/gcov 提供兼容包装 | 由覆盖率任务自动调用 |
| `devcontainer_setup.sh` | Dev Container 初始化配置 | 只在容器环境执行 |

## 编写新脚本的建议

1. 提供简短的 `print_usage`/`--help` 输出
2. 避免硬编码路径，优先使用项目根目录的相对路径
3. 将公共逻辑抽为函数，保持文件 <300 行
4. 在更新脚本后同步此表格，避免重复或过时脚本滞留
