# 快速开始

## 环境要求

- C++23 编译器（GCC 13+ 或 Clang 17+ 最低）
- CMake 3.28+
- Conan 2.x
- Linux 原生支持；macOS 可用 `./scripts/core/install-deps`（Homebrew）后构建；Windows 用 Docker 或 WSL

## 安装 Conan

FastQTools 用 Conan 2.x 管理 C++ 依赖。`scripts/core/install-deps` 不会安装 Conan，需要先手动安装：

```bash
pipx install conan        # 或 pip install conan
conan profile detect      # 生成默认 profile
```

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

v4 生产和 benchmark 构建只保留 Sequential 与 oneTBB backend；历史 Taskflow 对照数据仍保存在性能归档中，不再作为可选依赖构建。GitHub Actions 质量流水线 push/PR 均触发，sanitizer 矩阵随 PR 与主分支运行。

如需构建 benchmark，可在已有构建目录中启用 `-DBUILD_BENCHMARKS=ON`；benchmark、nlohmann_json 和 GoogleTest 由 Conan 构建选项按需加入。安装后库消费者只需链接 `FastQTools::FastQTools`，无需引入 cxxopts 或 nlohmann_json。

## 首次运行

仓库不附带样本数据，先生成（默认 10,000 条 reads、80–150 bp，seed=42 可复现）：

```bash
python3 scripts/datagen/gen_fastq.py -o sample.fastq
gzip -kf sample.fastq   # 生成 sample.fastq.gz，供下面的示例使用
```

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
- 在 C++ 项目中使用库：[api.md](./api.md)
- 架构与设计决策：[architecture.md](./architecture.md)
