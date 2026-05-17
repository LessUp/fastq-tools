# 部署指南

部署 FastQTools 的关键，不是把所有场景都抽象成一套平台，而是根据运行边界选择最小可维护方案：本地脚本、容器封装，或 CI/批处理流水线。

## 场景一：本地或共享节点直接运行

这是最直接的方式，适合开发、排障和小规模批处理：

```bash
./scripts/core/install-deps --runtime
./scripts/core/build
./build/clang-release/FastQTools --help
```

这种模式的优点是透明、调试成本低；缺点是依赖与你的机器环境绑定更紧。

## 场景二：容器化交付

如果你需要更稳定的运行边界，优先考虑仓库现有 Docker 入口：

- `docker/Dockerfile.dev`：面向开发与调试；
- `docker/Dockerfile.prod`：面向精简运行时；
- `docker/docker-compose.yml`：用于组合常见服务与挂载。

典型思路是把输入目录只读挂载，把输出目录单独挂载为可写位置，并让容器内命令保持与本地 CLI 一致。

## 场景三：CI 与自动化流程

在 CI 中，推荐把任务拆成三个阶段：

1. 构建：`./scripts/core/build` 或 `./scripts/core/build --dev`
2. 验证：`./scripts/core/test`
3. 文档 / 发布补充：按需执行 benchmark、文档构建或镜像步骤

对于只做数据处理的流水线，通常不需要在部署层重新包装命令；直接把 `FastQTools stat ...` / `FastQTools filter ...` 作为可审计步骤更清晰。

## 部署时最值得提前决定的事

| 决策点 | 你需要回答的问题 |
| --- | --- |
| 输入输出边界 | 数据从哪里来，结果写到哪里去？ |
| 并发策略 | 线程数是否由宿主机、调度器或脚本控制？ |
| 日志策略 | 需要调试信息，还是只保留错误输出？ |
| 证据留存 | 是否同时保存 `stat` 结果、sidecar 或 benchmark 记录？ |

## 与其他页面的关系

- 如果你还没有真正运行过一次命令，先回到[`快速开始`](./getting-started)
- 如果你正在查具体参数，回到[`CLI 参考`](./cli-reference)
- 如果你关心构建与测试的维护细节，继续读[`构建指南`](../dev/build)与[`测试策略`](../dev/testing)
