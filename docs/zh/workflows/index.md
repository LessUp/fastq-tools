# 工作流

工作流页负责把“我下一步要完成什么”翻译成具体阅读路径，让你从白皮书叙事自然进入执行层材料。

## 先选目标，再选页面

| 当前目标 | 建议路径 |
| --- | --- |
| 判断值不值得试 | [`为什么选择 FastQTools`](../why-fastqtools/) → [`技术白皮书`](../whitepaper/) → [`性能总览`](../performance/) |
| 快速跑通命令 | [`快速开始`](../guide/getting-started) → [`CLI 参考`](../guide/cli-reference) |
| 接入现有流程 | [`配置说明`](../guide/configuration) → [`部署指南`](../guide/deployment) |
| 进入开发与维护 | [`开发者文档`](../dev/) → [`参考导航`](../reference/) |

## 典型阅读路径

### 采用评估

先用 [`为什么选择 FastQTools`](../why-fastqtools/) 确认问题是否匹配，再读 [`架构`](../architecture/) 和 [`性能总览`](../performance/) 判断这些主张是否站得住。

### 执行落地

如果你已经准备运行命令，就从 [`快速开始`](../guide/getting-started) 进入，并在 [`CLI 参考`](../guide/cli-reference) 与 [`配置说明`](../guide/configuration) 中补齐参数与环境约束。

### 集成与维护

如果目标是嵌入 C++ 应用或给仓库提交补丁，请从 [`开发者文档`](../dev/) 与 [`参考导航`](../reference/) 进入，再回看 [`架构`](../architecture/) 对齐边界。

## 与白皮书的关系

白皮书负责建立判断框架，工作流负责把判断转化为动作。看完本页后，你应该知道自己是继续下钻执行文档，还是回到证据层补充上下文。
