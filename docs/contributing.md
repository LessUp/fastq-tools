# 贡献指南

欢迎为 FastQTools 项目做出贡献！

## 快速开始

详细的贡献指南请参阅项目根目录的 [CONTRIBUTING.md](https://github.com/LessUp/fastq-tools/blob/master/CONTRIBUTING.md)。

## 贡献方式

### 1. 报告问题

- 使用 [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) 报告 bug 或提出功能请求
- 提供详细的复现步骤和环境信息

### 2. 提交代码

```bash
# 1. Fork 仓库
# 2. 创建特性分支
git checkout -b feat/my-feature

# 3. 编写代码和测试
# 4. 运行验证
./scripts/core/lint format
./scripts/core/test

# 5. 提交并推送
git commit -m "feat: add my feature"
git push origin feat/my-feature

# 6. 创建 Pull Request
```

### 3. 改进文档

- 修正拼写/语法错误
- 添加使用示例
- 改进指南和教程
- 翻译文档

### 4. 代码审查

- 审查开放的 Pull Requests
- 提供建设性反馈
- 验证测试通过

## 开发环境设置

```bash
# 安装依赖
./scripts/core/install-deps

# 构建项目
./scripts/core/build

# 运行测试
./scripts/core/test
```

## 代码规范

- 遵循 [C++ 编码规范](dev/coding-standards.md)
- 使用 `./scripts/core/lint format` 格式化代码
- 添加必要的测试
- 更新相关文档

## 提交约定

使用 [Conventional Commits](https://www.conventionalcommits.org/) 格式：

```
feat|fix|docs|refactor|test|build|ci|chore(scope): subject
```

## 行为准则

请遵守 [Code of Conduct](https://github.com/LessUp/fastq-tools/blob/master/CODE_OF_CONDUCT.md)。

感谢你的贡献！🎉
