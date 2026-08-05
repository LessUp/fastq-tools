---
id: 001
title: stat 机器可读 JSON 输出
status: open
labels: [enhancement]
created: 2026-07-28
closed:
---

## 现象

`stat` 当前只输出 TSV：摘要行（`#ReadNum`/`#MaxReadLength`/`#Q20` 等）加逐位置明细表，以及可选的签名 sidecar（也是 TSV）。没有机器可读的 JSON 输出。

影响：下游流水线接入 QC 指标时必须自行解析 TSV；README 对比表中也需如实标注与 fastp（HTML + JSON 报告）的差距。这是已知缺口，不是路线图承诺。

## 根因

（关闭时填写）

## 处理

建议方向：为 `stat` 增加 JSON 输出选项（如 `--json <path>`），内容与 TSV 摘要/逐位置表保持同一指标集；nlohmann_json 已是按构建选项启用的 Conan 依赖，需评估是否引入 CLI 依赖面及其对二进制体积的影响。

## 复盘

（关闭时填写）
