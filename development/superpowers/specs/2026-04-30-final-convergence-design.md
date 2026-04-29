# FastQTools 最终收敛设计文档

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 将 FastQTools 项目从当前多分支、多 worktree、大量未提交改动的状态，收敛至单一干净的主线分支，达到可归档的工业级稳定状态。

**Architecture:** 顺序收敛策略：功能提交 → CI 精简合并 → 文档/AI 治理清理 → 最终验证归档。每个阶段完成后验证再进入下一阶段。

**Tech Stack:** C++23, CMake, Conan, GitHub Actions, MkDocs, Git Worktrees

---

## 当前状态

### 代码库状态
- **主分支 master**: 领先 origin/master 13 提交
- **未提交改动**: 34 文件，+874/-109 行
  - Advanced Preprocessing 功能（quality trimmer, poly-tail trimmer, adapter trimmer）
  - QC Signatures 功能（signature sidecar, duplicate estimation）
  - CLI 命令集成（`--signature-report`, `--adapter-seq`, `--trim-poly-g` 等）
  - 测试覆盖（单元测试、集成测试、E2E 测试）
  - 文档和规范更新

### Worktree 状态
- **`.worktrees/final-closeout`**: 绑定 `chore/final-closeout` 分支
- **未提交改动**: 大量修改 + 删除 3 个 CI 工作流（benchmark/valgrind/release）

### CI/CD 状态
- **当前工作流**: 5 个（ci, benchmark, release, valgrind, pages）
- **final-closeout 已删除**: benchmark, release, valgrind
- **决策**: 激进精简，仅保留 ci.yml 和 pages.yml

### 文档状态
- `docs/index.en.md`: 重复内容（115-153 行）
- `docs/superpowers/`: 位置不当，应移动到项目根目录
- `changelog/`: 22 个文件，部分应归档

### AI 治理文件状态
- `AGENTS.md`: 469 行，高质量，保留
- `QWEN.md`: 299 行，冗余 77%，删除
- `CLAUDE.md`: 79 行，冗余 60%，精简为 50 行
- `.github/copilot-instructions.md`: 10 行，冗余 80%，精简为 8 行
- `.windsurf/rules.md`: 27 行，冗余 60%，精简为 20 行

### Git Pages 决策
- **风格**: 技术深度型
- **首页区块**: 架构概览图 + 性能基准数据 + 快速开始
- **架构图**: 静态 SVG
- **性能数据**: 对比表格（vs seqtk/fastp）
- **语言**: 中英双语

---

## Phase 1: 功能提交 → 3.2.0 发布

### 目标
将当前 master 的 34 文件修改提交为 v3.2.0

### 任务清单

#### Task 1.1: 代码格式化
- [ ] 运行 `./scripts/core/lint format`
- [ ] 验证格式化结果：`./scripts/core/lint check`

#### Task 1.2: 构建验证
- [ ] 构建 Debug 版本：`./scripts/core/build --preset clang-debug`
- [ ] 验证构建成功

#### Task 1.3: 测试验证
- [ ] 运行所有测试：`./scripts/core/test`
- [ ] 验证测试通过

#### Task 1.4: 版本号更新
- [ ] 修改 `CMakeLists.txt` 中的 `PROJECT_VERSION` 为 `3.2.0`
- [ ] 验证版本号：`grep PROJECT_VERSION CMakeLists.txt`

#### Task 1.5: 提交变更
- [ ] 暂存所有改动：`git add .`
- [ ] 提交：`git commit -m "feat(release): v3.2.0 - advanced preprocessing and QC signatures"`
- [ ] 推送到远端：`git push origin master`

### 验收标准
- [ ] 所有测试通过
- [ ] 代码格式符合 `.clang-format`
- [ ] 版本号为 3.2.0
- [ ] 远端 master 已更新

---

## Phase 2: 合并 final-closeout → CI 精简

### 目标
将 final-closeout 的 CI 精简改动合并到 master，删除 worktree

### 任务清单

#### Task 2.1: 合并分支
- [ ] 切换到 master：`git checkout master`
- [ ] 合并 final-closeout：`git merge chore/final-closeout --no-ff -m "chore(ci): merge final-closeout - simplify CI workflows"`
- [ ] 解决冲突（如有）

#### Task 2.2: 验证 CI 配置
- [ ] 检查 `.github/workflows/` 目录
- [ ] 确认仅保留 `ci.yml` 和 `pages.yml`
- [ ] 验证 `ci.yml` 配置正确

#### Task 2.3: 清理 worktree
- [ ] 删除 worktree：`git worktree remove .worktrees/final-closeout`
- [ ] 删除分支：`git branch -d chore/final-closeout`
- [ ] 验证清理结果：`git worktree list` 和 `git branch -vv`

#### Task 2.4: 推送合并结果
- [ ] 推送到远端：`git push origin master`
- [ ] 验证远端状态：`git fetch --prune origin && git branch -vv`

### 验收标准
- [ ] master 包含所有 CI 精简改动
- [ ] worktree 已删除
- [ ] 远端分支已清理
- [ ] CI 工作流仅保留 ci.yml 和 pages.yml

---

## Phase 3: 文档/AI 治理清理 → Git Pages 重构

### 目标
清理冗余文档、精简 AI 治理文件、重构 Git Pages 为技术深度型

### 3.1 文档清理

#### Task 3.1.1: 修复 docs/index.en.md
- [ ] 删除 115-153 行的重复内容
- [ ] 修正语言标识
- [ ] 统一底部导航区块

#### Task 3.1.2: 移动 docs/superpowers/
- [ ] 创建目录：`mkdir -p development/superpowers`
- [ ] 移动文件：`mv docs/superpowers/* development/superpowers/`
- [ ] 删除空目录：`rmdir docs/superpowers`
- [ ] 更新 `.gitignore`（如需要）

#### Task 3.1.3: 归档 changelog
- [ ] 创建目录：`mkdir -p changelog/archive/2026`
- [ ] 移动旧文件：将 2026-04-22 及以前的文件移动到 `changelog/archive/2026/`
- [ ] 验证主目录仅保留近期条目

### 3.2 AI 治理精简

#### Task 3.2.1: 删除 QWEN.md
- [ ] 删除文件：`rm QWEN.md`

#### Task 3.2.2: 精简 CLAUDE.md
- [ ] 重写为 50 行快捷入口
- [ ] 保留：定位声明、Claude 使用方式、项目速览、高价值提醒、常用命令

#### Task 3.2.3: 精简 .github/copilot-instructions.md
- [ ] 重写为 8 行
- [ ] 保留：状态声明、MCP 说明、链接到 AGENTS.md

#### Task 3.2.4: 精简 .windsurf/rules.md
- [ ] 重写为 20 行
- [ ] 保留：描述、脚本入口、链接

### 3.3 Git Pages 重构

#### Task 3.3.1: 创建架构 SVG 图
- [ ] 设计架构图：CLI → Processing → I/O，Statistics 分支
- [ ] 创建 SVG 文件：`docs/assets/images/architecture.svg`
- [ ] 在首页引入

#### Task 3.3.2: 创建性能对比表格
- [ ] 收集基准数据：FastQTools vs seqtk vs fastp
- [ ] 创建表格：throughput、内存、功能覆盖
- [ ] 在首页展示

#### Task 3.3.3: 优化快速开始区块
- [ ] 精简安装命令
- [ ] 添加 5 分钟上手示例
- [ ] 添加常用 CLI 命令

#### Task 3.3.4: 中英双语支持
- [ ] 确保 mkdocs.yml 配置双语切换
- [ ] 验证所有页面都有中英文版本

### 验收标准
- [ ] Git Pages 构建成功
- [ ] 首页展示架构图、性能表格、快速开始
- [ ] AI 治理文件总量减少 25%+
- [ ] 双语切换正常

---

## Phase 4: 最终验证 → 归档

### 目标
确保项目达到可归档的工业级稳定状态

### 任务清单

#### Task 4.1: 全量测试
- [ ] 运行覆盖率测试：`./scripts/core/test --coverage`
- [ ] 验证覆盖率 ≥ 80%

#### Task 4.2: 静态分析
- [ ] 运行完整检查：`./scripts/core/lint all -b build/clang-debug`
- [ ] 修复所有警告

#### Task 4.3: Git Pages 部署验证
- [ ] 触发部署：推送改动
- [ ] 验证访问：https://lessup.github.io/fastq-tools/
- [ ] 验证所有链接

#### Task 4.4: GitHub 元数据更新
- [ ] 更新 description：`gh repo edit --description "..."`
- [ ] 验证 topics 正确
- [ ] 验证 homepage URL

#### Task 4.5: 清理工作树
- [ ] 清理未跟踪文件：`git clean -fd`
- [ ] 验证工作树干净：`git status`

#### Task 4.6: 创建归档标签
- [ ] 创建标签：`git tag -a v3.2.0-final -m "Final stable release"`
- [ ] 推送标签：`git push origin v3.2.0-final`

### 验收标准
- [ ] 所有测试通过
- [ ] 覆盖率 ≥ 80%
- [ ] Git Pages 正常访问
- [ ] 本地工作树干净
- [ ] 远端仅有 master 分支

---

## 风险与缓解

### 风险 1: 合并冲突
- **概率**: 中
- **影响**: 中
- **缓解**: 使用 `--no-ff` 合并，逐文件解决冲突

### 风险 2: CI 工作流删除后影响发布
- **概率**: 低
- **影响**: 高
- **缓解**: 保留 `ci.yml` 覆盖核心质量门禁，手动触发 benchmark

### 风险 3: Git Pages 重构后链接失效
- **概率**: 低
- **影响**: 中
- **缓解**: 使用 MkDocs 的 `use_directory_urls: true`，保持 URL 结构

---

## 时间估算

| Phase | 预计时长 | 依赖 |
|-------|---------|------|
| Phase 1 | 30 分钟 | 无 |
| Phase 2 | 20 分钟 | Phase 1 |
| Phase 3 | 60 分钟 | Phase 2 |
| Phase 4 | 30 分钟 | Phase 3 |
| **总计** | **140 分钟** | - |

---

## 成功标准

1. **代码**: master 分支干净，所有测试通过，覆盖率 ≥ 80%
2. **CI**: 仅保留 ci.yml 和 pages.yml
3. **文档**: Git Pages 技术深度型首页，中英双语
4. **AI 治理**: 文件总量减少 25%，无冗余
5. **Git**: 仅保留 master 分支，无 worktree，v3.2.0-final 标签

---

## 后续维护建议

1. **单人项目极简分支流**: 直接在 master 开发，高风险任务用 worktree
2. **CI 策略**: 保持 ci.yml + pages.yml，benchmark 改为手动触发
3. **文档维护**: 每季度审查，归档旧条目
4. **AI 治理**: 仅维护 AGENTS.md，补充文件保持快捷入口
