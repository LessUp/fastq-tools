# IO 权限与依赖清理及 stat 异常处理（2026-01-15）

## 背景
- 输出文件默认权限偏宽松，且项目未使用 bzip2/xz 仍声明依赖。
- `stat` 命令未捕获异常，报错信息不够明确。

## 本次变更
- 编辑 `src/io/fastq_writer.cpp`
  - 输出文件默认权限从 `0666` 调整为 `0644`。
- 编辑 `CMakeLists.txt`
  - 移除未使用的 `BZip2` 与 `LibLZMA` 查找。
- 编辑 `conanfile.py`、`config/dependencies/conanfile.py`
  - 移除 `bzip2` 与 `xz_utils` 依赖。
- 编辑 `src/cli/commands/stat_command.cpp`
  - 为统计执行逻辑补充 `try/catch`，输出错误并返回非 0。

## 影响范围
- 输出文件默认权限更严格。
- 构建依赖减少，Conan 配方更精简。
- `stat` 命令异常信息更清晰。

## 兼容性
- 不影响正常功能，构建依赖需同步更新缓存。

## 回退方案
- 恢复相应文件中的权限值、依赖声明与异常处理逻辑。
