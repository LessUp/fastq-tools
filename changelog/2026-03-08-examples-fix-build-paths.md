# examples 目录检查与修复

**日期**: 2026-03-08
**类型**: fix(examples)

## 变更内容

### 修复构建产物搜索路径
- 4 个示例脚本中的构建产物搜索路径从 `build-{compiler}-{type}` 修正为 `build/{compiler}-{type}`
- 与 CMakePresets.json 中 `binaryDir: ${sourceDir}/build/${presetName}` 保持一致
- 搜索优先级调整为 gcc-release → clang-release → gcc-debug → clang-debug（生产推荐优先）
- 影响文件: basic_stats.sh, basic_filter.sh, quality_trim.sh, pipeline_demo.sh
