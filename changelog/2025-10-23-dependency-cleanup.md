# 2025-10-23 依赖清理与 DevContainer 修复

## 变更

- 移除过时的 vcpkg 配置，统一为 Conan-only 工具链
- 更新 CMake 预设：移除 vcpkg 选项，对齐覆盖率/测试与 Conan profiles
- 修复 DevContainer 可用性：规范化 compose/dev 配置，确保脚本稳定运行

## 影响

- 依赖管理统一为 Conan，减少配置分叉
