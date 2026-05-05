# FastQTools 最终完结状态交接文档

## 交接日期
2026-04-26 22:10 UTC+8

## 项目当前状态

### 完成度
- **Phase 1（激进的审查与规范化重构）**：85% 完成
  - ✅ 激进的 baseline 真值对齐（openspec/baseline/）
  - ✅ Benchmark 主路径统一（docs/performance/benchmark-reports）
  - ✅ 版本锚点修正（工具链描述、CMakePresets.json）
  - ✅ 仓库 URL 元数据修复
  - ⏳ 文档树深度清理（docs-prune-core - 待细化）
  - ⏳ Changelog 压缩（changelog-compress - 待实施）
  - ⏳ Pages 重构（pages-reframe - 设计中）
  - ⏳ Benchmark 策略重置（benchmark-policy-reset - 待启动）

- **Phase 2（工程化与 GitHub 深度集成）**：5% 规划完成
  - 🚧 CI workflow 精简（ci-workflow-reduce）
  - 🚧 GitHub about 更新（github-about-sync）
  - 🚧 开发流程固化（dev-flow-freeze）

- **Phase 3（AI 工具链配置）**：0% 启动
  - 📋 Copilot 指令初始化
  - 📋 AGENTS.md 与 CLAUDE.md 深度配置
  - 📋 LSP 规则配置
  - 📋 MCP/Skill 权衡评估
  - 📋 Plugin 表面评估

- **Phase 4（收尾计划）**：待制定

### 最新提交
```
commit 3f622e0
Author: Claude + Copilot
Date:   2026-04-26 22:10

refactor: phase 1 baseline truth realignment + benchmark path unification

32 files changed, 988 insertions(+), 612 deletions(-)
```

---

## 关键决策与约束

### 单人项目工作流
- ✅ **已实施**：直接 push 流程（无强制 PR 门禁）
- ✅ **已固化**：`scripts/core/` 是唯一官方入口（构建、测试、lint、依赖）
- ✅ **已明确**：OpenSpec 驱动的变更门槛（proposal vs. direct implementation）
- 🔲 **待文档化**：GitHub `gh` CLI 集成实践
- 🔲 **待配置**：CODEOWNERS / GitHub branch rules

### 版本与工具链政策
- ✅ **已调整**：从"硬性统一 GCC 15 / Clang 21"改为"首选 profile + 本地自适应"
- ✅ **已对齐**：Baseline 与实际构建脚本、Conan profile 一致
- 🔲 **待封板**：Docker 构建映像版本锚定（GCC 15.x 作为生产标准）
- 🔲 **待验证**：跨平台（musl / glibc / aarch64）构建矩阵

### 压缩与文件格式
- ✅ **已确认**：维护中仅支持 gzip（zlib + libdeflate）
- ✅ **已同步**：baseline schema、依赖、文档一致
- 🔲 **待回溯**：文档中残留的 bzip2 / xz 参考（存在于 archive/ 中，无需改）

---

## 立即可执行的后续任务

### Priority 1: Phase 1 收尾（1-2 小时）
1. **docs-prune-core**：
   - 删除 `docs/api/` 中的过时 .md 文件（已有 openspec/baseline/api/ 为准）
   - 清理 `docs/dev/` 中的冗余设计文档（保留 build / testing / quality-tools / git-guidelines）
   - 统一 mkdocs.yml nav 结构，删除未包含的页面引用

2. **changelog-compress**：
   - 归档 `changelog/archive/2025/` 下的所有条目到 `openspec/archive/`
   - 保留 `changelog/` 仅用于当前年份 / 活跃改动
   - 在 CHANGELOG.md 中写入统一的"近期变更汇总"

3. **pages-reframe**：
   - 重设计 GitHub Pages 主页（docs/index.en.md），突出 3 个核心亮点：性能、零拷贝、规范驱动
   - 更新 README.md 中的 GitHub Pages 链接为最新
   - 补充 docs/404.md 和 docs/404.en.md 的友好重定向

### Priority 2: Phase 2 启动（2-3 小时）
1. **ci-workflow-reduce**：
   - `.github/workflows/benchmark.yml`：合并为单个 PR trigger（移除手动工作流）
   - `.github/workflows/valgrind.yml`：改为定时 (weekly) 触发，不再 push 触发
   - `.github/workflows/ci.yml`：精简 matrix（GCC Release + Clang Release + Clang ASan 三个必要配置）
   - 删除未使用的 action / job（如果有）

2. **github-about-sync**：
   ```bash
   gh repo edit LessUp/fastq-tools \
     --description "High-performance FASTQ QC toolkit (stat/filter); zero-copy I/O, TBB pipeline, C++23" \
     --homepage "https://lessup.github.io/fastq-tools/" \
     --topics "bioinformatics,fastq,cpp23,performance,quality-control"
   ```

3. **dev-flow-freeze**：
   - 在 `docs/dev/git-guidelines.md` 中写入统一的本地工作流示例
   - 补充 `CONTRIBUTING.md` 中对"proposal threshold"的实际判断方法

### Priority 3: Phase 3 启动（需要 Claude Code 或 Copilot）
1. **ai-governance-refit**：
   - 确保 `AGENTS.md` 中已有完整的 Spec-Driven Development 工作流说明
   - 补充 `CLAUDE.md` 中的"当前项目矛盾点速查表"（如有）
   - 验证 `QWEN.md` 和 `.windsurf/rules.md` 内容无冗余

2. **lsp-tooling-finalize**：
   - `.clangd` 配置（如有）
   - `compile_commands.json` 由 CMake 自动生成（已配置）
   - 验证 include path、standard 选项正确

3. **mcp-skill-tradeoff**：
   - 评估哪些仓库特有的工作流适合做成 CLI Skill（如 benchmark aggregation）
   - 确认 MCP 不需要引入（本项目规模无需 MCP）

---

## 待审视的架构问题

### 文档站点导航（mkdocs.yml）
当前结构可能过于扁平。建议：
```yaml
nav:
  - Home: index.md
  - Get Started: guide/getting-started.md
  - CLI Reference: guide/cli-reference.md
  - API Reference:
      - Overview: api/overview.md
      - I/O: api/io.md
      - Processing: api/processing.md
  - Developer:
      - Build: dev/build.md
      - Architecture: dev/architecture.md
      - Coding Standards: dev/coding-standards.md
  - Performance: performance/benchmark-report.md
```

### OpenSpec 提案流程（openspec/changes/）
- `final-state-convergence/` 已建立
- 后续新提案应遵循相同结构：design.md / proposal.md / tasks.md
- 落地后记得移到 `openspec/archive/YYYY/`

### 代码库本身
- 无重大架构腐化（zero-copy I/O、TBB pipeline 设计清晰）
- baseline API 与实现一致
- 测试覆盖完整（unit/integration/e2e）
- **已知改进点**：
  - benchmark 工具脚本可进一步模块化（非紧急）
  - Docker 多阶段构建可优化（非紧急）

---

## 文件变更总结

### 新增
- `changelog/2026-04-26-baseline-truth-realignment.md`
- `changelog/2026-04-26-final-state-convergence-proposal.md`
- `openspec/changes/final-state-convergence/`（3 个文件）

### 修改（27 个文件）
- `openspec/baseline/` 中的 4 个核心文档（大幅重写）
- `docs/` 中的 11 个文档（工具链描述、路径调整）
- 构建配置与工具脚本（CMakeLists.txt、CMakePresets.json、benchmark 脚本等）
- CI workflow（benchmark.yml）

### 无需改的存留
- `docs/archive/2026-04-17-migrated-to-specs/` - 旧规范存档（保留参考）
- `README.md / README.zh-CN.md` - 已验证无漂移
- 源代码（include/fqtools/, src/）- 无改动

---

## 验证清单（交接前已完成）
- ✅ `git diff --check` - 无空白/格式错误
- ✅ `mkdocs build --strict` - 文档站点编译通过
- ✅ `git log --oneline -1` - 提交信息清晰
- 🔲 `./scripts/core/lint format` - 可选（无 C++ 改动）
- 🔲 `./scripts/core/test` - 可选（无功能改动）

---

## 给 GLM 的建议

### 优先顺序
1. **立即执行** Phase 1 收尾（docs-prune / changelog / pages）
2. **并行进行** Phase 2 的 ci-workflow-reduce（工作流改动无依赖）
3. **深入讨论** Phase 3 的 AI 治理（这部分需要对项目长期愿景的理解）

### 风险点
- **Pages 重构**：需要美学与信息架构的权衡（建议保守设计）
- **CI 精简**：valgrind / benchmark 的触发频率需要权衡可见性与成本
- **AI 配置**：AGENTS.md 过长可能降低易用性（建议模块化与链接）

### 交接后续
- 如有重大设计变更，应在 `openspec/changes/` 中先提案
- 每次提交应参考当前 `CONTRIBUTING.md` 中的工作流
- 文档改动应经 `mkdocs build --strict` 验证
- 定期检查 `changelog/` 是否及时更新

---

## 快速参考

### 核心命令
```bash
# 开发构建
./scripts/core/build --dev

# 运行测试
./scripts/core/test --unit

# 代码质量
./scripts/core/lint format

# 文档验证
python3 -m mkdocs build --strict

# 依赖更新
./scripts/core/install-deps --all
```

### 文件入口
- 规范驱动：`AGENTS.md` → `openspec/baseline/` → `openspec/changes/`
- 构建系统：`CMakeLists.txt` + `CMakePresets.json` + `scripts/core/`
- 文档站点：`mkdocs.yml` + `docs/` + GitHub Pages
- 变更记录：`changelog/` （年份目录） + `CHANGELOG.md`

### 关键目录
```
openspec/
├── baseline/     # 事实来源（产品、API、架构、测试）
├── changes/      # 活跃提案
└── archive/      # 已落地提案的历史

scripts/core/
├── build         # 构建脚本
├── test          # 测试脚本
├── lint          # 质量脚本
└── install-deps  # 依赖管理

docs/
├── guide/        # 用户指南
├── dev/          # 开发者指南
├── api/          # API 参考（与 openspec/baseline/api 对应）
└── performance/  # 性能数据

changelog/
├── 2026/         # 当前年份变更
└── archive/      # 历史记录
```

---

## 反馈与问题

如遇以下情况，请回溯到本文档的对应章节：
- **工作流冲突**：见"关键决策与约束"
- **任务阻塞**：见"立即可执行的后续任务"
- **设计疑问**：见"待审视的架构问题"
- **提交规范**：见 CONTRIBUTING.md + AGENTS.md

---

**交接人**：Claude Haiku (Copilot CLI)  
**完成度**：Phase 1 ~85%，可独立交接给 GLM 继续推进  
**预期后续周期**：Phase 1 收尾 1-2h / Phase 2 2-3h / Phase 3 深化 / Phase 4 确认 → 可交付的"最终完结状态"
