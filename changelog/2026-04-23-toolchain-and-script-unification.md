# 本地工具链与脚本约定统一

## 目标

统一本地构建/测试/lint 的默认入口，减少 `scripts/core/*`、`CMakePresets.json`、VS Code 设置之间的约定漂移。

## 主要改动

- 让 `scripts/core/build` 的默认构建目录与 CMake preset 命名保持一致，并修正 `--preset` 的实际行为。
- 为 `clang-ubsan`、`clang-msan` 增加 preset，并让 coverage 默认落到 `build/coverage`。
- 修正 `build/test/lint/install-deps` 脚本中的旧入口示例（`./scripts/*` → `./scripts/core/*`）。
- 为 `test` / `lint` 增加 `--preset` 入口，并改进编译数据库目录发现逻辑。
- 在 `CMakeLists.txt` 中补齐 `ENABLE_LTO` 和 `ENABLE_CLANG_TIDY`，使脚本选项与 CMake 行为一致。
- 为 VS Code 设定默认 `clang-debug` preset，并自动复制 `compile_commands.json` 到仓库根目录。
- 移除 `scripts/core/lint` 中对 `/tmp` 的依赖，改为内存中处理 clang-tidy 输出。

## 影响范围

- 本地开发入口：`scripts/core/build`、`scripts/core/test`、`scripts/core/lint`
- 构建配置：`CMakeLists.txt`、`CMakePresets.json`
- 编辑器默认行为：`.vscode/settings.json`

## 验证方式

```bash
./scripts/core/build --preset clang-debug --clean
./scripts/core/build --coverage --clean
./scripts/core/test --unit -b build/clang-debug
./scripts/core/test --unit --preset coverage
./scripts/core/lint check
cmake --list-presets
```
