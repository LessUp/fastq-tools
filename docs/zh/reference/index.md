---
title: 参考导航
description: 在白皮书叙事之后，快速落到命令、API、实现材料与研究索引。
layout: doc
---

<SectionLandingGrid locale="zh" section="reference" />

# 参考导航

参考导航是面向“我已经知道自己要查什么”的读者准备的。白皮书负责建立系统叙事，参考导航负责让你在不重复阅读整套叙事的情况下，直接找到精确命令、精确接口与精确维护材料。

## 先把深内容层分清楚

新版内容栈最有效的用法，是让每一层都只承担一个任务：

- [`白皮书`](../whitepaper/) 负责范围、约束与采用逻辑；
- [`架构`](../architecture/) 负责解释为什么这些层和边界存在；
- [`算法`](../algorithms/) 负责把过滤、统计、执行与内存行为写成维护中的系统说明；
- [`性能总览`](../performance/) 负责解释吞吐证据应该如何阅读；
- [`研究附录`](../research/) 负责补充书目、相关项目比较与演进说明。

参考导航出场的时机，就是这些叙事已经足够清楚之后。

## 实施路径

当任务不再是“理解故事”，而是“落到精确接口”时，就应该进入参考导航。它的职责，是把读者引导到命令、API、构建与贡献者界面，而不是继续重复叙事。

## 操作参考

- [`CLI 参考`](../guide/cli-reference)
- [`快速开始`](../guide/getting-started)
- [`配置说明`](../guide/configuration)
- [`部署指南`](../guide/deployment)

## CLI 与 API

- [`API 概览`](../api/overview)
- [`IO 模块`](../api/io)
- [`处理模块`](../api/processing)
- [`统计模块`](../api/statistics)

## 贡献者界面

- [`开发者文档`](../dev/)
- [`开发者架构设计`](../dev/architecture)
- [`构建`](../dev/build)
- [`测试`](../dev/testing)
- [`Benchmark 指南`](../dev/benchmark-guide)

## 研究与比较入口

- [`研究附录`](../research/)
- [`书目`](../research/bibliography)
- [`相关项目`](../research/related-projects)
- [`演进说明`](../research/evolution-notes)
- [`归档概览`](../archive/)

## 白皮书重构后的站内锚点

<ReferenceList locale="zh" variant="site" />

## 如何使用这一页

- 想立刻执行命令：从操作参考开始；
- 想理解公共接口与库集成：从 CLI 与 API 开始；
- 想排查内部设计、构建与测试：从贡献者界面开始；
- 想补充项目背景、外部文献或对照材料：转到研究附录。
