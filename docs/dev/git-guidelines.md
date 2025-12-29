# Git 提交规范

## 提交格式

```
<type>(<scope>): <subject>
```

## Type 类型

| 类型 | 描述 |
|------|------|
| `feat` | 新功能 |
| `fix` | Bug 修复 |
| `docs` | 文档更改 |
| `style` | 代码风格（不影响功能） |
| `refactor` | 代码重构 |
| `perf` | 性能优化 |
| `test` | 添加或修改测试 |
| `build` | 构建系统或依赖更改 |
| `ci` | CI/CD 配置更改 |
| `chore` | 其他维护性更改 |

## Scope 范围（可选）

- `core`: 核心功能
- `cli`: 命令行接口
- `parser`: 解析器
- `devops`: 运维相关
- `docs`: 文档相关

## Subject 主题

- 使用祈使句，现在时态
- 第一个字母不要大写
- 结尾不加句号

## 示例

```
feat(parser): add BAM file input support
fix(core): correct GC content calculation
docs(readme): update installation instructions
refactor(pipeline): simplify processing loop
perf(io): optimize buffer allocation
ci(github): add automated linting
```

## 提交信息模板

```bash
# 仓库内模板文件：.gitmessage.txt
# 可在本地执行：git config commit.template .gitmessage.txt
# Dev Container 会自动设置该配置。

feat(cli): add new filter option
fix(io): handle empty file case
docs: improve API documentation
refactor: extract common utility functions
```

## 注意事项

- 提交信息要简洁明了
- 每个提交只做一件事
- 避免过于宽泛的描述
- 使用英文撰写提交信息
