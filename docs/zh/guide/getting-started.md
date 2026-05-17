# 快速开始

这份快速开始以“第一次成功执行”为目标，而不是覆盖所有功能。完成下面流程后，你应该能在本地构建 FastQTools、查看帮助，并对 `stat` / `filter` 两类工作负载有一个清晰印象。

## 成功标准

当你完成本页时，应该已经做到：

1. 在仓库根目录准备好依赖；
2. 构建出可执行文件；
3. 成功查看 CLI 帮助；
4. 知道下一步该去哪里查命令、配置与部署细节。

## 环境准备

在仓库根目录执行：

```bash
./scripts/core/install-deps
./scripts/core/build --dev
```

如果你只关心运行时验证，也可以使用默认 Release 构建：

```bash
./scripts/core/build
```

项目默认使用 `scripts/core/*` 作为统一入口，这样可以减少手动维护 Conan、CMake 目录与 preset 细节的成本。

## 第一次确认 CLI 可用

构建完成后，先确认二进制存在，再查看帮助：

```bash
./build/clang-debug/FastQTools --help
```

如果你刚刚执行的是默认 Release 构建，请改用：

```bash
./build/clang-release/FastQTools --help
```

你会看到全局日志选项以及 `stat`、`filter` 两个核心子命令。到这里为止，你已经验证了“工具能启动、命令入口存在”。

## 认识第一批工作流

### `stat`：先建立证据

`stat` 负责把 FASTQ 输入转化为长度、碱基组成、质量分布等统计证据。它适合放在你正式过滤、调参或写报告之前。

```bash
./build/clang-debug/FastQTools stat -i reads.fastq.gz -o stats.txt
```

### `filter`：再执行清洗

`filter` 负责把质量阈值、长度阈值、修剪与预处理策略应用到输入数据上。

```bash
./build/clang-debug/FastQTools filter -i reads.fastq.gz -o filtered.fastq.gz           --min-quality 20           --min-length 50           --trim-quality 20           --trim-mode both
```

上面两条命令不要求你现在就记住全部参数；真正的目标是让你知道项目的使用节奏通常是“先看证据，再决定过滤与部署策略”。

## 下一步怎么走

- 想查完整命令与选项：继续看[`CLI 参考`](./cli-reference)
- 想理解配置优先级与推荐写法：继续看[`配置说明`](./configuration)
- 想把工具放进容器、CI 或批处理环境：继续看[`部署指南`](./deployment)
- 想深入内部设计：转到[`开发者文档`](../dev/)
