# 覆盖率阈值检查：百分比解析健壮性修复（2026-01-13）

## 背景
- `scripts/tools/coverage-report` 在阈值检查阶段会把覆盖率百分比转换为 basis points。
- 原实现对输入缺少校验：当百分比字符串为空、格式不合法或包含非数字字符时，bash 算术展开会报错并在 `set -euo pipefail` 下导致脚本直接退出。

## 本次变更
- 编辑：
  - `scripts/tools/coverage-report`
    - `percent_to_basis_points()` 增加输入格式校验（仅接受 `^[0-9]+(\.[0-9]*)?$`），失败时返回非 0。
    - 阈值比较处捕获解析失败：解析失败仅输出告警并跳过比较，避免脚本崩溃。

## 影响范围
- `./scripts/tools/coverage-report --check-threshold` 在异常/脏输入时更健壮，不再因 bash 算术解析错误提前退出。

## 兼容性
- 对正常的 lcov 输出格式无行为变化。

## 回退方案
- 如需回退，可恢复 `scripts/tools/coverage-report` 中阈值检查函数到变更前版本。
