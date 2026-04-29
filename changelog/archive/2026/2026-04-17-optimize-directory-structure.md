# 2026-04-17-optimize-directory-structure

## 目标
优化项目目录结构，提升组织清晰度和可维护性

## 主要改动

### 1. 移除无关文件
- **删除 `antissh.sh`**：该脚本（1891 行 graftcp/代理配置）与 FASTQ 项目无关，应归属于个人 dotfiles 仓库

### 2. 重新组织文档目录
- **移动 `docs/benchmark-reports/` → `docs/performance/benchmark-reports/`**
  - 将基准测试报告移至 performance 子目录，避免与文档源码混淆
- **移动 `RELEASE_NOTES.md` → `changelog/releases/v3.1.0.md`**
  - 消除 CHANGELOG.md、changelog/、RELEASE_NOTES.md 三处重复
  - 建立清晰的发布版本归档结构

### 3. 清理空目录
- **删除 `docs/assets/images/`**：空目录，无实际内容

### 4. 重新分类脚本工具
将 `scripts/tools/` 下的 12 个脚本按功能分类到三个子目录：

#### `scripts/tools/performance/` - 性能测试与分析
- `benchmark` - 性能基准测试 CLI
- `benchmark-io` - I/O 性能基准测试
- `valgrind-cachegrind` - 缓存性能分析

#### `scripts/tools/analysis/` - 代码分析与测试
- `coverage-report` - 覆盖率报告生成
- `valgrind-memcheck` - 内存泄漏检测
- `run-fuzzer` - Fuzz 测试运行器
- `generate-diff` - 代码差异报告生成

#### `scripts/tools/release/` - 发布与部署
- `deploy` - Docker/生产环境部署
- `package-release` - 发布打包工具
- `install-llvm` - LLVM 工具链安装
- `setup-devcontainer` - DevContainer 初始化

### 5. 更新文档引用
- 更新 `mkdocs.yml` 导航路径
- 更新 `docs/dev/benchmark-guide.md` 和 `.en.md` 中的路径
- 更新 `docs/performance/benchmark-report.md` 中的路径
- 更新 `docs/guide/deployment.md` 和 `.en.md` 中的脚本路径
- 更新 `scripts/tools/README.md` 以反映新的分类结构

## 影响范围
- 目录结构：更清晰的项目组织
- 文档：所有引用已更新
- 脚本：路径变更，需要适应新的位置
- 无功能性代码变更

## 验证方式
- `git status` 查看所有移动和修改
- 确认文档链接正确
- 确认脚本可执行

## 新目录结构

```
scripts/tools/
├── performance/        # 性能测试与分析
│   ├── benchmark
│   ├── benchmark-io
│   └── valgrind-cachegrind
├── analysis/           # 代码分析与测试
│   ├── coverage-report
│   ├── valgrind-memcheck
│   ├── run-fuzzer
│   └── generate-diff
├── release/            # 发布与部署
│   ├── deploy
│   ├── package-release
│   ├── install-llvm
│   └── setup-devcontainer
└── README.md

docs/
├── performance/
│   ├── benchmark-report.md
│   └── benchmark-reports/    # 基准测试报告
│       ├── baselines/
│       ├── data/
│       ├── results/
│       └── compiler/
└── ...

changelog/
├── releases/               # 发布版本归档
│   └── v3.1.0.md
└── ...
```
