# 2025-12-26 清理 AI 自动生成任务清单（去伪存真）

## 背景

`tasks-template.md` 中包含大量 AI 自动生成的“看似专业但不贴合现状/重复/引入不必要复杂度”的条目。
本次变更对其中关键任务项进行了对照仓库现状的审计与清理，并补齐可落地的需求/设计/进度文档。

## 变更

- **任务清单清理（tasks-template.md）**
  - 将已在仓库中实现的条目改为 ✅ 已完成，并修正文件路径：
    - FASTQ 零拷贝数据结构（`FastqRecord`/`FastqBatch`）。
    - 日志初始化（`fq::logging`）。
    - Release workflow 与打包脚本。
    - LTO（Release preset 已启用 `-flto` + IPO）。
  - 将“过早优化/过度承诺”的性能项降级为“测量驱动的调优任务”，避免在无数据情况下引入 SIMD/预取等复杂度。
  - **删除** PGO 相关任务：流程复杂、收益不确定，且会显著增加构建/发布维护成本。
  - 将覆盖率/静态分析从“一上来就强门禁”调整为“先报告可见，后续再收敛为门禁”。

- **新增文档（真实可落地）**
  - `docs/dev/requirements.md`
  - `docs/dev/design.md`
  - `docs/dev/implementation_progress.md`

## 备注

本次清理旨在减少“AI 生成任务幻觉”对迭代节奏的干扰，使后续开发围绕可验证目标推进。
