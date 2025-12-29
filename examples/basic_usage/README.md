# FastQTools 基本使用示例

本目录包含 FastQTools 的基本使用示例，帮助用户快速上手。

## 示例列表

### 1. 基本统计分析
- **文件**: `basic_stats.sh`
- **功能**: 演示如何对 FastQ 文件进行基本统计分析
- **适用**: 初学者

## 快速开始

### 准备测试数据

```bash
# 本仓库内置示例数据（推荐）
ls ../../tools/data/*.fastq

# 例如：
# - ../../tools/data/sample_10k.fastq
# - ../../tools/data/sample_100k_len100.fastq
```

### 运行示例

```bash
# 先构建 FastQTools
../../scripts/build.sh

# 基本统计
./basic_stats.sh ../../tools/data/sample_10k.fastq
```

## 预期输出

每个示例都会生成相应的统计报告文件，包含：

- 读取数量和长度分布
- 碱基组成和 GC 含量
- 质量分数分布
- 错误率估算

## 自定义参数

所有示例都支持通过环境变量自定义参数：

```bash
# 设置线程数
export FASTQTOOLS_THREADS=8

# 设置批处理大小
export FASTQTOOLS_BATCH_SIZE=100000

# 运行示例
./basic_stats.sh input.fastq.gz
```

## 注意事项

1. 确保 FastQTools 可执行文件可用：
   - 已安装在 PATH 中，或
   - 已在仓库根目录执行 `./scripts/build.sh`，或
   - 设置 `FASTQTOOLS=/path/to/FastQTools`
2. 示例脚本需要可执行权限：`chmod +x *.sh`
3. 某些示例可能需要较大内存，建议在配置充足的机器上运行
4. 处理大文件时，建议使用 SSD 存储以提高性能

## 故障排除

如果遇到问题，请检查：

1. FastQTools 版本是否兼容
2. 输入文件格式是否正确
3. 系统资源是否充足
4. 权限设置是否正确

更多帮助请参考 [使用指南](../../docs/user/usage.md) 或提交 [Issue](https://github.com/LessUp/FastQTools/issues)。
