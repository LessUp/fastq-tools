# fmt 头文件模式修复 consteval 编译失败（2026-01-08）

## 变更
- `config/dependencies/conanfile.py`
  - 将 `fmt/11.0.2` 切换为 `header_only=True`，避免在 GCC 15/Clang 21 下编译 fmt 源码时触发 consteval 相关构建失败。

## 影响
- Conan 将重新生成 `fmt` 的包 ID（头文件模式），跳过编译阶段，解决 `FMT_STRING` consteval 报错。
- 若已有旧的 `fmt/11.0.2` 编译产物缓存，建议删除对应 Conan 缓存包或重新执行 `conan install` 以获取新的 header-only 变体。
