# 更新日志

本项目所有重要变更均记录在此处。

## 最新版本

查看完整的变更日志，请访问 GitHub 仓库：

- [完整 Changelog](https://github.com/LessUp/fastq-tools/blob/master/CHANGELOG.md)
- [详细变更记录](https://github.com/LessUp/fastq-tools/tree/master/changelog/)

## 变更格式

我们使用基于日期的变更记录格式：

```
changelog/
├── 2026-04-17-optimize-directory-structure.md
├── 2026-04-17-fix-github-workflows.md
└── ...
```

每个变更文件包含：
- 目标和范围
- 主要改动
- 影响范围
- 验证方式

## Conventional Commits

提交信息遵循 [Conventional Commits](https://www.conventionalcommits.org/) 规范：

```
feat|fix|docs|refactor|test|build|ci|chore(scope): subject
```

示例：
- `feat(cli): add filter command`
- `fix(io): resolve memory leak in fastq reader`
- `docs: update getting started guide`
