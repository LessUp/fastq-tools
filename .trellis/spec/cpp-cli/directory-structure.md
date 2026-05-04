# Directory Structure

> 规范源：[AGENTS.md - 项目结构](../../../AGENTS.md#项目结构)

---

## 关键约定速查

### 公共 API vs 实现

| 位置 | 内容 |
|------|------|
| `include/fqtools/` | 公共 API 头文件（稳定接口） |
| `src/` | 实现代码 |

### 核心模块

| 目录 | 用途 |
|------|------|
| `io/` | FASTQ I/O，批处理与零拷贝记录视图 |
| `processing/` | 过滤、修剪、并行流水线 |
| `statistics/` | 统计计算逻辑 |
| `error/` | 异常层次结构 |
| `common/` | 工具类 |

### 测试组织

| 目录 | 内容 |
|------|------|
| `tests/unit/` | 单元测试（镜像 `src/` 结构） |
| `tests/integration/` | 集成测试 |
| `tests/e2e/` | CLI 端到端测试 |

### 文件命名

- 头文件：`lower_case.h`
- 实现文件：`lower_case.cpp`
- 测试文件：`test_<module>.cpp`
