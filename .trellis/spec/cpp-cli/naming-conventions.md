# Naming Conventions

> 规范源：[AGENTS.md - 代码风格指南 - 命名约定](../../../AGENTS.md#代码风格指南)

---

## 关键约定速查

### 命名规则表

| 类型 | 规则 | 示例 |
|------|------|------|
| 类 / 结构体 | PascalCase | `FastqBatch`, `StatCommand` |
| 函数 / 方法 | camelCase | `validateLengths()`, `getPassRate()` |
| 变量 / 参数 | camelCase | `totalReads`, `inputFile` |
| 私有成员 | camelCase_ | `config_`, `pipeline_` |
| 常量 / constexpr | kCamelCase | `kDefaultBatchSize` |
| 枚举值 | PascalCase | `CompressionType::Gzip` |
| 命名空间 | lower_case | `fq::processing` |

### 命名空间约定

```cpp
namespace fq {}           // 主命名空间
namespace fq::io {}       // I/O 模块
namespace fq::processing {}  // 处理模块
namespace fq::error {}    // 错误处理
```

### 文件命名

| 类型 | 规则 | 示例 |
|------|------|------|
| 头文件 | lower_case.h | `fastq_reader.h` |
| 实现文件 | lower_case.cpp | `fastq_reader.cpp` |
| 测试文件 | test_<module>.cpp | `test_io.cpp` |

### 强制执行

由 `.clang-tidy` 的 `readability-identifier-naming` 检查强制执行。运行 `./scripts/core/lint tidy` 检查违规。
