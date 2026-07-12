# CLAUDE.md — FastQTools 项目速查

> AI Agent 速查表。权威规则见 `AGENTS.md`。

## 项目

- **FastQTools**: C++23 高性能 FASTQ 处理工具，工程能力展示项目。
- 公共 API 在 `include/fqtools/`，实现在 `src/`。
- 并行基于 `tbb::parallel_pipeline`，零拷贝 `std::string_view`。

## 常用命令

```bash
./scripts/core/build          # 构建（默认 Clang Release）
./scripts/core/build --dev    # Debug 模式
./scripts/core/test           # 全部测试
./scripts/core/test --unit    # 单元测试
./scripts/core/lint format    # 格式化
./scripts/core/lint check     # 检查格式
```

## 提醒

- 修改 C++ 源码后至少运行 `lint format` + 相关测试。
- 注释、commit message 优先中文。
- 不用 `std::endl`；统一 `"\n"`。
