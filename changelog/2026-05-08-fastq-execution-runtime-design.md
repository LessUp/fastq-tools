# FASTQ execution runtime 设计文档落盘

## 目标

把共享 FASTQ 批处理执行运行时的架构设计写成仓库内可审阅文档，并补齐相关上下文词汇。

## 主要改动

- 新增 `CONTEXT.md`，记录 **FASTQ execution runtime** 这一项目特定术语，并澄清它与现有 “pipeline” 说法的区别。
- 新增 `docs/superpowers/specs/2026-05-08-fastq-execution-runtime-design.md`，记录运行时深化设计、适配器形状、调用侧收缩方式与测试策略。

## 影响范围

- 仓库级上下文词汇
- 架构设计文档

## 验证方式

- `git diff --check`

## 备注

- no baseline delta
