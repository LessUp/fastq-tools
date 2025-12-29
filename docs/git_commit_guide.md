# Git 提交规范指引

为了保持项目提交历史的可读性与一致性，本项目采用 **Angular 规范** 的变体。

## 1. 提交格式

```text
<type>(<scope>): <subject>

<body> (可选)
<footer> (可选)
```

### Type (类型)
- **feat**: 新功能 (feature)
- **fix**: 修补 bug
- **docs**: 文档 (documentation)
- **style**: 格式 (不影响代码运行的变动)
- **refactor**: 重构 (既不是新增功能，也不是修改 bug 的代码变动)
- **perf**: 提高性能的优化
- **test**: 增加测试
- **build**: 影响构建系统或外部依赖的更改 (如: cmake, vcpkg)
- **ci**: 更改 CI 配置和脚本
- **chore**: 非 src 或测试文件的修改 (如: gitignore)
- **revert**: 回滚到上一个版本

### Scope (作用域)
可选，描述变动影响的模块（如：`io`, `stats`, `cli`, `config`）。

### Subject (主题)
简洁的中文描述。

---

## 2. 如何配置

### 设置本地模板
在项目根目录运行：
```bash
git config --local commit.template .gitmessage.txt
```

### 强制校验 (可选)
您可以将以下代码保存为 `.git/hooks/commit-msg` 以进行自动校验：

```bash
#!/bin/sh
commit_msg_file=$1
commit_msg=$(cat "$commit_msg_file")
pattern="^(feat|fix|docs|style|refactor|perf|test|build|ci|chore|revert)(\(.*\))?: .+"

if ! echo "$commit_msg" | grep -qE "$pattern"; then
    echo "错误: 提交信息格式不符合规范！"
    echo "正确格式: <type>(<scope>): <subject>"
    exit 1
fi
```
