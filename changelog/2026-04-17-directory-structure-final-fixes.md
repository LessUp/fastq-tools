# 2026-04-17-directory-structure-final-fixes

## 目标
完成目录结构优化的最终修复

## 主要改动

### 1. 修复 .gitignore 路径错误
- **修复 `docs/benchmark-reports/` → `docs/performance/benchmark-reports/`**
  - .gitignore 中的路径与实际目录结构不匹配
  - 现在正确忽略 results/*.json 等生成文件
  - 从 git 跟踪中移除 benchmark 结果 JSON 文件

### 2. 修复重复文件
- **将 `latest.json` 改为符号链接**
  - 之前是完整复制，导致与基线文件完全重复
  - 现在使用符号链接指向实际的结果文件
  - 更符合 Unix 最佳实践

### 3. 整理松散文件
- **整理 `docs/archive/` 目录**
  - 将 4 个松散文件移动到 `docs/archive/2025-12-29-audit/` 子目录
  - 保持归档目录的一致组织结构

## 影响范围
- .gitignore：正确忽略生成的基准测试文件
- 基准测试报告：使用符号链接而非复制
- 文档归档：更清晰的组织结构
- 无功能性代码变更

## 验证方式
- `git status` 确认更改
- 确认 benchmark 结果文件不再被跟踪
- 确认符号链接正确
