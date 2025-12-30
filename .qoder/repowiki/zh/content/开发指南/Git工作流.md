# Git工作流

<cite>
**本文档中引用的文件**  
- [.gitattributes](file://.gitattributes)
- [.gitignore](file://.gitignore)
- [.git-blame-ignore-revs](file://.git-blame-ignore-revs)
- [commitlint.config.js](file://commitlint.config.js)
- [docs/git_commit_guide.md](file://docs/git_commit_guide.md)
- [CONTRIBUTING.md](file://CONTRIBUTING.md)
- [scripts/build.sh](file://scripts/build.sh)
- [scripts/test.sh](file://scripts/test.sh)
- [scripts/lint.sh](file://scripts/lint.sh)
</cite>

## 目录
1. [简介](#简介)
2. [分支管理策略](#分支管理策略)
3. [提交规范](#提交规范)
4. [.gitattributes 文件说明](#gitattributes-文件说明)
5. [.gitignore 排除规则](#gitignore-排除规则)
6. [.git-blame-ignore-revs 用途](#git-blame-ignore-revs-用途)
7. [典型工作流示例](#典型工作流示例)
8. [代码审查与CI流水线](#代码审查与ci流水线)

## 简介

本指南旨在为 `fastq-tools` 项目提供一套标准化的Git工作流实践，确保团队协作高效、代码质量可控。通过明确分支策略、提交规范和工具链集成，帮助开发者遵循一致的开发流程。

**Section sources**
- [CONTRIBUTING.md](file://CONTRIBUTING.md#L1-L45)

## 分支管理策略

本项目采用**功能分支模式**（Feature Branch Workflow）进行版本控制。所有开发活动应在独立的功能分支中进行，主分支 `main` 仅用于发布稳定版本。

- `main` 分支：受保护分支，仅允许通过合并请求（Pull Request）更新，代表最新的稳定发布状态。
- 功能分支（如 `feature/new-cli-command`）：从 `main` 分支切出，用于实现新功能或修复缺陷。
- 修复分支（如 `hotfix/critical-bug`）：紧急情况下从 `main` 创建，用于快速修复生产问题。

所有功能开发完成后，需通过Pull Request提交代码审查，并在CI流水线通过后方可合并至 `main`。

**Section sources**
- [CONTRIBUTING.md](file://CONTRIBUTING.md#L13-L14)

## 提交规范

为保证提交历史清晰可读，项目要求所有提交信息必须符合**约定式提交**（Conventional Commits）规范，并通过 `commitlint` 自动校验。

### 提交格式

```text
<类型>(<作用域>): <简要描述>

<详细说明>（可选）
<脚注>（可选）
```

### 类型（Type）

- **feat**: 新功能
- **fix**: 修复缺陷
- **docs**: 文档变更
- **style**: 代码格式调整（不影响逻辑）
- **refactor**: 代码重构
- **perf**: 性能优化
- **test**: 测试相关修改
- **build**: 构建系统或依赖变更
- **ci**: CI配置修改
- **chore**: 日常维护任务
- **revert**: 回滚提交

### 作用域（Scope）

可选字段，用于标识变更影响的模块，例如 `io`、`cli`、`config` 等。

### 配置提交模板

建议在本地配置提交模板以提高效率：

```bash
git config --local commit.template .gitmessage.txt
```

**Section sources**
- [docs/git_commit_guide.md](file://docs/git_commit_guide.md#L1-L58)
- [commitlint.config.js](file://commitlint.config.js#L1-L2)

## .gitattributes 文件说明

`.gitattributes` 文件用于定义Git如何处理不同类型的文件，确保跨平台一致性。

主要配置包括：

- **行尾符标准化**：所有文本文件统一使用 `LF` 换行符，避免Windows与Unix系统间的换行差异。
- **二进制文件标记**：图像、压缩包、可执行文件等被标记为 `binary`，防止Git尝试进行文本合并。
- **语言特定规则**：C/C++、CMake、Shell脚本等均明确指定为文本文件并使用 `LF`。

此配置有助于避免因编辑器或操作系统差异导致的无意义变更。

**Section sources**
- [.gitattributes](file://.gitattributes#L1-L46)

## .gitignore 排除规则

`.gitignore` 文件定义了不应提交到版本控制中的文件和目录，防止误提交敏感信息或构建产物。

### 排除逻辑

- **编译输出**：忽略所有对象文件（`.o`）、静态库（`.a`）、动态库（`.so`、`.dll`）及构建目录（如 `/build/`, `/cmake-build-*`）。
- **临时文件**：忽略 `.tmp`、`.log`、`.swp` 等临时文件。
- **IDE配置**：忽略 `.vscode/`（除 `settings.json` 外）、`.idea/` 等IDE生成文件。
- **依赖管理**：忽略 Conan 生成的 `conaninfo.txt`、`conan.lock` 等文件。
- **分析报告**：忽略覆盖率数据（`.gcda`, `.gcno`）、Clang-Tidy修复文件等。
- **操作系统文件**：忽略 `.DS_Store`、`Thumbs.db` 等系统生成文件。

该规则设计兼顾通用性与项目特性，确保仓库整洁且安全。

**Section sources**
- [.gitignore](file://.gitignore#L1-L86)

## .git-blame-ignore-revs 用途

`.git-blame-ignore-revs` 文件用于记录那些不应出现在 `git blame` 输出中的提交哈希，通常用于大规模格式化或机械性重构提交。

- 当执行全项目代码格式化时，这些提交会污染 blame 历史，使代码追溯变得困难。
- 将此类提交的哈希添加到该文件后，可通过以下命令忽略它们：
  ```bash
  git blame --ignore-revs-file .git-blame-ignore-revs <file>
  ```
- 此机制有助于保持代码责任追溯的准确性，仅关注有意义的逻辑变更。

目前该文件为空，待首次大规模格式化后将填入相应提交哈希。

**Section sources**
- [.git-blame-ignore-revs](file://.git-blame-ignore-revs#L1-L4)

## 典型工作流示例

以下是完整的功能开发工作流示例：

1. **同步主分支**
   ```bash
   git checkout main
   git pull origin main
   ```

2. **创建功能分支**
   ```bash
   git checkout -b feature/new-io-optimization
   ```

3. **开发与阶段性提交**
   - 编写代码、运行本地构建与测试
   - 使用 `scripts/lint.sh` 检查代码风格
   - 提交时遵循提交规范，例如：
     ```
     feat(io): implement buffered FASTQ reader
     ```

4. **本地验证**
   ```bash
   ./scripts/build.sh
   ./scripts/test.sh
   ./scripts/lint.sh
   ```

5. **推送并创建Pull Request**
   ```bash
   git push origin feature/new-io-optimization
   ```
   在GitHub上发起Pull Request，关联相关议题（Issue）。

**Section sources**
- [scripts/build.sh](file://scripts/build.sh#L1-L319)
- [scripts/test.sh](file://scripts/test.sh#L1-L271)
- [scripts/lint.sh](file://scripts/lint.sh#L1-L198)

## 代码审查与CI流水线

在发起Pull Request后，必须满足以下条件方可合并：

- **CI流水线通过**：包括构建、单元测试、代码覆盖率、静态分析等阶段。
- **至少一名团队成员批准审查**：重点关注代码逻辑、性能影响、接口设计。
- **提交信息规范**：每个提交都需通过 `commitlint` 校验。
- **文档更新**：若涉及用户可见变更，需同步更新相关文档。

CI系统将自动运行测试套件，确保变更不会破坏现有功能。开发者应在合并前解决所有CI失败项。

**Section sources**
- [CONTRIBUTING.md](file://CONTRIBUTING.md#L15-L18)
- [scripts/test.sh](file://scripts/test.sh#L1-L271)