# 快速开始

本指南帮助你在最短时间内完成 FastQTools 的环境搭建并运行第一个命令。

---

## 系统要求

### 运行时（最低）

- Ubuntu 20.04+ 或 Debian 11+
- 1GB RAM / 100MB 磁盘

### 开发环境（推荐）

- Ubuntu 22.04+ 或 Debian 12+
- 4GB RAM / 2GB 磁盘
- 多核 CPU（加速并行构建）

---

## 安装依赖

```bash
# 开发环境（包含编译器、构建工具、调试工具）
./scripts/core/install-deps

# 仅运行时依赖（生产部署）
./scripts/core/install-deps --runtime

# 查看帮助
./scripts/core/install-deps --help
```

**开发依赖包含：**

- GCC 15 / Clang 21 编译器
- CMake、Ninja 构建系统
- Conan 2.x 包管理器
- GDB、Valgrind 调试工具
- lcov 覆盖率工具

**运行时依赖包含：**

- libtbb12（Intel TBB 并行库）
- zlib、bzip2、xz 压缩库
- libdeflate（高性能解压）

---

## 编译构建

```bash
# 默认构建（Clang + Release）
./scripts/core/build

# 开发模式（Debug + 详细输出）
./scripts/core/build --dev

# 指定编译器和构建类型
./scripts/core/build --compiler gcc --type Debug

# 查看所有选项
./scripts/core/build --help
```

构建成功后，可执行文件位于 `build/clang-release/FastQTools`（或对应构建目录）。

---

## 验证安装

```bash
# 查看帮助
./build/clang-release/FastQTools --help

# 运行测试
./scripts/core/test
```

---

## 第一个示例

### 1. 准备测试数据

```bash
# 使用工具生成测试 FASTQ 文件
python3 tools/data/gen_fastq.py -o test_data.fastq --reads 5000 --min-len 80 --max-len 150
```

### 2. 统计分析

```bash
FastQTools stat -i test_data.fastq -o stats.txt --threads 4
```

输出包含：读段总数、长度分布、碱基组成（A/T/C/G/N）、GC 含量、Q20/Q30 百分比等。

### 3. 质量过滤

```bash
FastQTools filter -i test_data.fastq -o filtered.fastq \
  --min-quality 20 \
  --min-length 50 \
  --max-n-ratio 0.1 \
  --trim-quality 20 \
  --trim-mode both
```

---

## 常见问题

### 依赖安装失败

- 确保系统为 Ubuntu/Debian 系列
- 手动更新包列表：`sudo apt-get update`
- 检查网络连接

### 构建失败

- 确认 Conan 已安装：`conan --version`
- 确认 CMake 可用：`cmake --version`
- 确认编译器可用：`clang++ --version` 或 `g++ --version`

### 找不到可执行文件

- 检查构建目录：`ls build/clang-release/FastQTools`
- 确认构建脚本成功完成（返回码为 0）

### 运行时缺少共享库

```bash
# 检查缺少的库
ldd build/clang-release/FastQTools

# 安装运行时依赖
./scripts/core/install-deps --runtime
```

---

## 下一步

- [CLI 参考](cli-reference.md) — 完整命令行用法
- [配置管理](configuration.md) — 环境变量与配置文件
- [部署指南](deployment.md) — Docker 与生产环境部署
- [构建指南](../dev/build.md) — 高级构建选项
