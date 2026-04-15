# 修复 generate-diff.sh 输出路径污染（2025-12-26）

## 背景
- `generate-diff.sh` 在创建输出目录时，通过 `echo` 打印提示，导致 stdout 同时包含提示语与目录路径。
- 主流程使用命令替换捕获函数返回值，结果提示语被拼入 `--output` 参数，`git diff` 因非法路径报错。

## 变更
- 将提示信息改为输出到 stderr，保证 stdout 仅返回目录路径。

## 测试
- `./scripts/generate-diff.sh`（选择 `a`，生成未提交变更的 diff）
