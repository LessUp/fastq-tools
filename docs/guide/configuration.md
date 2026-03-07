# 配置管理

FastQTools 支持通过配置文件、环境变量和命令行参数控制运行行为，三者的优先级从低到高依次为：

**默认值 → 配置文件 → 环境变量 → 命令行参数**

---

## 命令行参数

```bash
# 键值对格式
FastQTools stat --input=reads.fq.gz --output=stats.txt

# 分离格式
FastQTools stat -i reads.fq.gz -o stats.txt

# 短选项
FastQTools stat -i reads.fq.gz -o stats.txt -t 8
```

### 短选项映射

| 短选项 | 长选项 | 说明 |
|--------|--------|------|
| `-i` | `--input` | 输入文件 |
| `-o` | `--output` | 输出文件 |
| `-t` | `--threads` | 线程数 |
| `-v` | `--verbose` | 详细日志 |
| `-q` | `--quiet` | 静默模式 |

---

## 环境变量

以 `FASTQTOOLS_` 为前缀的环境变量会自动加载为配置项（前缀去掉后转为小写）：

```bash
export FASTQTOOLS_THREADS=8
export FASTQTOOLS_BATCH_SIZE=100000

FastQTools stat -i input.fastq -o output.txt
# threads=8, batch_size=100000 自动生效
```

命令行参数可覆盖环境变量：

```bash
export FASTQTOOLS_THREADS=8
FastQTools stat -i input.fastq -o output.txt --threads 16
# 实际使用 threads=16
```

---

## 配置验证

`Configuration::validate()` 执行以下检查：

- 必需键（`input`, `output`）是否存在
- 数值范围有效性（如 `threads` 必须在 1–256 之间）
- 配置一致性

验证失败时抛出 `fq::error::ConfigurationError` 异常。

---

## 构建时配置

### CMake 选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `ENABLE_COVERAGE` | 启用代码覆盖率 | OFF |
| `BUILD_BENCHMARKS` | 构建基准测试 | OFF |
| `BUILD_TESTING` | 构建单元测试 | ON |
| `CMAKE_BUILD_TYPE` | 构建类型 | Release |

```bash
cmake -B build -DENABLE_COVERAGE=ON -DBUILD_TESTING=ON
```

### Conan 依赖

依赖定义在 `config/dependencies/conanfile.py`：

| 包 | 版本 | 用途 |
|----|------|------|
| `cxxopts` | 3.1.1 | 命令行解析 |
| `spdlog` | 1.17.0 | 日志 |
| `fmt` | 12.1.0 | 格式化 |
| `zlib-ng` | 2.3.2 | gzip 压缩 |
| `nlohmann_json` | 3.11.3 | JSON 处理 |
| `onetbb` | 2022.3.0 | 并行计算 |
| `libdeflate` | 1.25 | 高性能解压 |

---

## Docker 环境变量

```bash
# 生产环境
FASTQTOOLS_DATA_DIR=/app/data
FASTQTOOLS_OUTPUT_DIR=/app/output

# 开发环境
CC=clang
CXX=clang++
CONAN_HOME=/home/developer/.conan2
CCACHE_DIR=/home/developer/.ccache
```

---

## 性能调参建议

| 参数 | 说明 | 建议 |
|------|------|------|
| `threads` | 并行线程数 | 设为 CPU 核心数 |
| `batchSize` | 每批 reads 数量 | 10000–100000 |
| `batchCapacityBytes` | 批次内存限制 | 根据可用内存调整 |
| `maxInFlightBatches` | 流水线并发批次数 | 2× 线程数 |
