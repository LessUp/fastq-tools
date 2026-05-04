# Code Style

> 规范源：[AGENTS.md - 代码风格指南](../../../AGENTS.md#代码风格指南)

---

## 关键约定速查

### 格式设置

| 设置 | 值 |
|------|------|
| 列宽 | 100 |
| 缩进 | 4 空格 |
| 大括号风格 | Attach |
| 指针对齐 | Left |

### Include 顺序

由 `.clang-format` 自动排序：

1. `"fqtools/..."` — 项目头文件
2. `<vector>`, `<string>` — C++ 标准库
3. 其他系统头文件

### 代码风格要点

```cpp
// 尾置返回类型
auto readRecord() -> FastqRecord;

// 左对齐指针
int* ptr;
const std::string& ref;

// [[nodiscard]] 用于查询函数
[[nodiscard]] auto isOpen() const -> bool;
```

### 禁止事项

- ❌ 头文件中使用 `using namespace`
- ❌ 使用 `std::endl`（用 `"\n"`）
- ❌ 单行 if/loop（保持大括号）

### 格式化命令

```bash
./scripts/core/lint check    # 检查格式
./scripts/core/lint format   # 自动格式化
```
