# Testing Guidelines

> 规范源：[AGENTS.md - 测试策略](../../../AGENTS.md#测试策略)

---

## 关键约定速查

### 测试组织

| 目录 | 内容 | 框架 |
|------|------|------|
| `tests/unit/` | 单元测试（镜像 `src/`） | GTest |
| `tests/integration/` | 跨模块集成测试 | GTest |
| `tests/e2e/` | CLI 端到端测试 | Bash + Python |

### 测试命名约定

| 类型 | 约定 | 示例 |
|------|------|------|
| 测试文件 | `test_<module>.cpp` | `test_io.cpp` |
| 测试类 | `<Module>Test` | `FastqReaderTest` |
| 测试用例 | `<Object>_<Scenario>_<Expected>` | `ReadBasic_ReturnsAllRecords` |

### 运行测试

```bash
./scripts/core/test              # 所有测试
./scripts/core/test --unit       # 单元测试
./scripts/core/test --coverage   # 覆盖率报告
```

### 测试工具

| 工具 | 功能 |
|------|------|
| `TempDirectory` | RAII 临时目录 |
| `FixtureLoader` | 加载测试数据 |
| `TestDataGenerator` | 生成 FASTQ 记录 |

### CTest 标签

| 标签 | 超时 |
|------|------|
| `unit` | 60s |
| `integration` | 120-180s |
| `e2e` | 180s |
