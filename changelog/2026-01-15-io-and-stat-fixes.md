# 2026-01-15 IO 权限、依赖清理与 stat 命令修复

> 本文件合并自 2 个同日同主题的独立 changelog 条目。

## 1. IO 权限与依赖清理及 stat 异常处理

### 变更
- `src/io/fastq_writer.cpp`：输出文件默认权限从 `0666` 调整为 `0644`。
- `CMakeLists.txt`：移除未使用的 `BZip2` 与 `LibLZMA` 查找。
- `conanfile.py`、`config/dependencies/conanfile.py`：移除 `bzip2` 与 `xz_utils` 依赖。
- `src/cli/commands/stat_command.cpp`：为统计执行逻辑补充 `try/catch`，输出错误并返回非 0。

## 2. 修复 stat 命令必需参数校验

### 变更
- `src/cli/commands/stat_command.cpp`：在解析后补充 `--input`/`--output` 必需参数检查，缺失时输出错误提示与帮助信息并返回非 0。

## 影响

- 输出文件默认权限更严格。
- 构建依赖减少，Conan 配方更精简。
- `FastQTools stat` 在缺失参数或异常时行为更明确。

## 兼容性

- 正常调用不受影响，构建依赖需同步更新缓存。
