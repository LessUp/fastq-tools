# 2025-12-25 CLI 命令接口重命名（破坏性变更）

- `src/cli/commands/i_command.h` -> `src/cli/commands/command_interface.h`
- `fq::cli::ICommand` -> `fq::cli::CommandInterface`
- 同步更新引用点：
  - `src/cli/commands/filter_command.h`
  - `src/cli/commands/stat_command.h`
  - `src/cli/main.cpp`
  - `src/cli/CMakeLists.txt`

