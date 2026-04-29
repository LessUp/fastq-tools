# GitHub 项目与文档优化

**日期**: 2026-04-22
**类型**: docs / chore

---

## 目标

全面检查并优化 GitHub 项目配置和 Git Pages 文档站点。

---

## 主要改动

### 1. GitHub Repository Description

- **Before**: 包含双语的长描述
- **After**: 简洁专业的英文描述
  ```
  High-Performance FASTQ Processing Toolkit (C++23 + TBB): QC Stats & Read Filtering for NGS Data
  ```

### 2. GitHub Topics 优化

- 新增精准标签: `sequence-analysis`, `ngs-qc`, `fastq-processor`
- 当前总计 20 个高质量 topics
- 覆盖生物信息学、C++、高性能计算等核心领域

### 3. GitHub Release v3.1.0

- 重构 release notes 格式：英文优先，中文次之
- 添加清晰的章节分隔
- 补充性能指标和平台支持详情
- 结构化展示核心功能、性能、开发工具、构建系统、Docker 支持

### 4. Workflows 分析结果

所有现有 workflows 均必要且设计合理：

| Workflow | 用途 | 结论 |
|----------|------|------|
| `ci.yml` | 核心构建与测试 | ✅ 必要 |
| `pages.yml` | 文档部署 | ✅ 必要 |
| `release.yml` | 多平台发布构建 | ✅ 必要 |
| `quality.yml` | 代码质量检查 (格式、静态分析、ASan、覆盖率) | ✅ 必要 |
| `benchmark.yml` | 性能监控 (每周运行) | ✅ 有价值 |
| `valgrind.yml` | 内存检查 (每周运行) | ✅ 有价值 |

### 5. Bug 检查

- 所有 13 个测试通过 (100% 通过率)
- 无关键 bug 发现
- Clang Release 构建成功

### 6. 文档状态

- README.md / README.zh-CN.md: 结构完善，内容全面
- docs/ 目录: 完整双语文档
- mkdocs.yml: 专业 Material 主题配置
- 自定义 CSS/JS: 精致用户体验

---

## 验证方式

```bash
# 检查 GitHub 项目配置
gh repo view --json description,repositoryTopics

# 检查 release
gh release view v3.1.0

# 本地构建文档
mkdocs build --strict

# 运行测试
ctest --test-dir build/clang-release --output-on-failure
```

---

## 影响范围

- GitHub 项目元数据 (description, topics)
- GitHub Release v3.1.0
- Workflows 配置 (确认全部保留)
- 测试验证 (全部通过)
