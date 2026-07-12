# 快速开始

## 环境要求

- C++23 编译器（GCC 11+ 或 Clang 12+ 最低）
- CMake 3.28+
- Conan 2.x
- Linux 或 macOS；Windows 用 Docker 或 WSL

## 安装依赖

```bash
./scripts/core/install-deps
```

## 构建

```bash
./scripts/core/build                  # 默认 Clang Release
./scripts/core/build --dev            # Debug + 详细输出
./scripts/core/build --compiler gcc   # GCC
./scripts/core/build --sanitizer asan # ASan
```

构建产物在 `build/clang-release/FastQTools`。

## 首次运行

```bash
./build/clang-release/FastQTools --help

# 统计
./build/clang-release/FastQTools stat -i sample.fastq.gz -o sample.stats.txt

# 过滤 + 修剪
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

## 下一步

- 命令参数细节：[cli-reference.md](./cli-reference.md)
- 嵌入 C++ 项目：[api.md](./api.md)
- 架构与设计决策：[architecture.md](./architecture.md)
