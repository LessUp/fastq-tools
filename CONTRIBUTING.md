# Contributing to FastQTools

业余练手项目，欢迎贡献。

## 开发流程

```bash
git status --short --branch    # 检查工作树
./scripts/core/build --dev      # 构建
./scripts/core/lint check       # 格式检查
./scripts/core/test             # 测试
```

- 单人项目，默认直接在当前分支改动。
- 修改 C++ 源码后至少运行 `lint format` 和相关测试。
- commit message 用 Conventional Commits：`feat|fix|docs|refactor|test|chore(scope): subject`。
- 注释优先中文。

## License

贡献内容按项目 LICENSE（MIT）授权。
