# tools/ 目录重构整理

**日期**: 2026-03-07
**类型**: refactor
**范围**: tools

## 变更内容

### 结构重组
- 在 `tools/benchmark/` 下新增 `scripts/` 子目录，将 Python/Shell 分析脚本与 C++ 基准测试源码分离
- 移动文件：`badge_generator.py`、`gen_benchmark_data.py`、`regression_detector.py`、`report_generator.py`、`run_baseline.sh` → `tools/benchmark/scripts/`

### 文档更新
- 重写 `tools/README.md`，修复与实际目录结构不符的错误描述
- 新增 `tools/benchmark/README.md`：C++ 基准测试和分析脚本的使用说明
- 新增 `tools/data/README.md`：测试数据文件说明和生成方法
- 新增 `tools/fuzz/README.md`：模糊测试构建、运行和种子文件说明

### 引用路径更新
- `.github/workflows/benchmark.yml`：更新 3 处 Python 脚本路径
- `scripts/tools/benchmark`：更新 `TOOLS_DIR` 变量
- `tools/benchmark/scripts/run_baseline.sh`：修复 `PROJECT_ROOT` 相对路径
- `docs/specs/benchmark-system/design.md`：更新代码注释中的路径
- `docs/specs/benchmark-system/tasks.md`：更新任务描述中的路径
- `docs/wiki/性能优化/基准测试方法.md`：批量更新 14 处路径引用
- `docs/wiki/性能优化/性能优化.md`：更新 4 处路径引用
- `docs/wiki/性能优化/SIMD优化策略.md`：更新 2 处路径引用
