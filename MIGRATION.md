# FastQTools 脚本系统迁移指南

本文档帮助你从旧的脚本系统迁移到新的统一脚本接口。

## 概述

FastQTools 进行了全面的脚本系统重构，提供了更清晰、更一致的接口。主要改进包括：

✅ **统一的命令接口** - 所有脚本使用一致的参数格式  
✅ **公共函数库** - 消除重复代码，提高可维护性  
✅ **更好的错误处理** - 详细的错误信息和恢复建议  
✅ **增强的文档** - 每个脚本都有完整的帮助信息  
✅ **改进的测试架构** - 新增集成测试层，更好的测试组织  

---

## 脚本映射表

### 构建脚本

| 旧命令 | 新命令 | 说明 |
|--------|--------|------|
| `./scripts/build.sh` | `./scripts/build` | 默认构建 |
| `./scripts/build.sh clang Release` | `./scripts/build` | 相同（默认就是 clang Release） |
| `./scripts/build.sh gcc Debug` | `./scripts/build -c gcc -t Debug` | 指定编译器和类型 |
| `./scripts/build.sh clang Debug --asan` | `./scripts/build -t Debug -s asan` | Sanitizer 支持 |
| `./scripts/build.sh --coverage` | `./scripts/build --coverage` | 覆盖率构建 |
| `./scripts/build-dev.sh` | `./scripts/build --dev` | 开发模式 |
| `./scripts/build-dev.sh gcc debug` | `./scripts/build --dev -c gcc` | 开发模式 + gcc |

### 测试脚本

| 旧命令 | 新命令 | 说明 |
|--------|--------|------|
| `./scripts/test.sh` | `./scripts/test` | 运行所有测试 |
| `./scripts/test.sh -c clang -t Debug` | `./scripts/test -c clang -t Debug` | 指定构建配置 |
| `./scripts/test.sh -C` | `./scripts/test --coverage` | 覆盖率测试 |
| `./scripts/test.sh -f "*timer*"` | `./scripts/test --filter "*timer*"` | 过滤测试 |
| `./scripts/test.sh -v` | `./scripts/test --verbose` | 详细输出 |
| `./scripts/test.sh -r 5` | `./scripts/test --repeat 5` | 重复测试 |
| N/A | `./scripts/test --unit` | 只运行单元测试（新功能） |
| N/A | `./scripts/test --integration` | 只运行集成测试（新功能） |

### 代码质量脚本

| 旧命令 | 新命令 | 说明 |
|--------|--------|------|
| `./scripts/lint.sh format` | `./scripts/lint format` | 格式化代码 |
| `./scripts/lint.sh format-check` | `./scripts/lint check` | 检查格式 |
| `./scripts/lint.sh lint` | `./scripts/lint tidy` | 静态分析 |
| `./scripts/lint.sh lint-fix` | `./scripts/lint tidy-fix` | 自动修复 |
| N/A | `./scripts/lint all` | 运行所有检查（新功能） |

### 依赖安装脚本

| 旧命令 | 新命令 | 说明 |
|--------|--------|------|
| `./scripts/install_deps.sh` | `./scripts/install-deps` | 安装开发依赖 |
| `./scripts/install_deps.sh --dev` | `./scripts/install-deps` | 相同 |
| `./scripts/install_deps.sh --runtime` | `./scripts/install-deps --runtime` | 只安装运行时 |
| `./scripts/install_runtime.sh` | `./scripts/install-deps --runtime` | 已合并 |
| N/A | `./scripts/install-deps --dry-run` | 预览安装（新功能） |

### 其他脚本

| 旧脚本 | 新脚本 | 状态 |
|--------|--------|------|
| `package_release.sh` | 待更新 | 保留但需要更新 |
| `docker_deploy.sh` | 待更新 | 保留但需要更新 |
| `gcov_wrapper.sh` | 保留 | 内部使用 |
| `devcontainer_setup.sh` | 保留 | 特定用途 |
| `benchmark_io.sh` | 保留 | 工具脚本 |

---

## 详细迁移步骤

### 步骤 1: 备份现有配置

如果你有自定义的构建配置或脚本：

```bash
# 备份你的修改
cp scripts/build.sh scripts/build.sh.backup
cp scripts/test.sh scripts/test.sh.backup
```

### 步骤 2: 更新脚本权限

新脚本需要可执行权限：

```bash
chmod +x scripts/build
chmod +x scripts/test
chmod +x scripts/lint
chmod +x scripts/install-deps
```

### 步骤 3: 测试新脚本

先在非关键环境测试：

```bash
# 测试构建
./scripts/build --help
./scripts/build --dev

# 测试测试脚本
./scripts/test --help
./scripts/test --unit

# 测试代码质量
./scripts/lint --help
./scripts/lint check
```

### 步骤 4: 更新 CI/CD 配置

#### GitHub Actions

**旧配置**:
```yaml
- name: Build
  run: ./scripts/build.sh clang Release

- name: Test
  run: ./scripts/test.sh -c clang -t Release -C
```

**新配置**:
```yaml
- name: Build
  run: ./scripts/build

- name: Test
  run: ./scripts/test --coverage
```

#### GitLab CI

**旧配置**:
```yaml
build:
  script:
    - ./scripts/build.sh gcc Debug
    
test:
  script:
    - ./scripts/test.sh -c gcc -t Debug
```

**新配置**:
```yaml
build:
  script:
    - ./scripts/build -c gcc -t Debug
    
test:
  script:
    - ./scripts/test -c gcc -t Debug
```

### 步骤 5: 更新文档和脚本引用

在项目文档中搜索旧脚本引用并更新：

```bash
# 查找需要更新的引用
grep -r "build.sh" docs/
grep -r "test.sh" docs/
grep -r "install_deps.sh" docs/
```

### 步骤 6: 更新开发者习惯

团队成员需要知道的改变：

1. **帮助信息**: 所有新脚本都支持 `--help`
   ```bash
   ./scripts/build --help
   ```

2. **开发模式**: 使用 `--dev` 快速开发
   ```bash
   ./scripts/build --dev
   ```

3. **测试分层**: 可以单独运行不同层次的测试
   ```bash
   ./scripts/test --unit        # 快速单元测试
   ./scripts/test --integration # 集成测试
   ./scripts/test --e2e         # 端到端测试
   ```

---

## 常见问题

### Q1: 旧脚本什么时候会被删除？

**A**: 旧脚本将在 **2 个版本周期**后（约 3-6 个月）被移除。在此期间它们会被移到 `scripts/deprecated/` 目录并标记为废弃。

### Q2: 如果新脚本不工作怎么办？

**A**: 
1. 先查看详细输出：`./scripts/build --verbose`
2. 检查 `scripts/lib/common.sh` 是否存在
3. 暂时可以回退到旧脚本
4. 报告问题到 issue tracker

### Q3: 如何在新旧脚本间切换？

**A**: 两套脚本可以共存。新脚本名称去掉了 `.sh` 后缀：
```bash
./scripts/build.sh    # 旧版本
./scripts/build       # 新版本
```

### Q4: 自定义的构建配置如何迁移？

**A**: 新脚本提供更多配置选项：

**旧方式**（硬编码）:
```bash
# 在 build.sh 中修改
BUILD_DIR="my-custom-dir"
```

**新方式**（命令行参数）:
```bash
./scripts/build --build-dir my-custom-dir
```

### Q5: 性能有变化吗？

**A**: 新脚本性能相同或更好：
- 更少的重复代码执行
- 更智能的缓存检查
- 可选的并行化

---

## 已知问题和解决方案

### 问题 1: "common.sh not found"

**症状**: 运行新脚本时报错找不到 `common.sh`

**解决方案**:
```bash
# 确保文件存在
ls scripts/lib/common.sh

# 如果不存在，重新拉取
git pull origin main
```

### 问题 2: 权限错误

**症状**: `Permission denied`

**解决方案**:
```bash
chmod +x scripts/lib/common.sh
chmod +x scripts/build
chmod +x scripts/test
chmod +x scripts/lint
chmod +x scripts/install-deps
```

### 问题 3: 参数不兼容

**症状**: 旧的参数格式不工作

**解决方案**: 查看帮助信息了解新格式
```bash
./scripts/build --help
```

---

## 回滚指南

如果需要临时回退到旧脚本：

### 选项 1: 使用旧脚本名称

```bash
# 旧脚本仍然可用
./scripts/build.sh
./scripts/test.sh
./scripts/lint.sh
```

### 选项 2: Git 回退

```bash
# 回退到重构前的版本
git checkout <pre-refactor-commit> -- scripts/
```

### 选项 3: 使用备份

```bash
# 如果你备份了
cp scripts/build.sh.backup scripts/build.sh
```

---

## 获取帮助

### 文档资源

- **脚本文档**: `scripts/README.new.md`
- **测试文档**: `tests/README.md`
- **API 文档**: 运行 `doxygen` 生成

### 命令帮助

每个脚本都有详细的帮助信息：
```bash
./scripts/build --help
./scripts/test --help
./scripts/lint --help
./scripts/install-deps --help
```

### 社区支持

- 提交 issue: [GitHub Issues](https://github.com/your-repo/fastq-tools/issues)
- 查看示例: `docs/dev/build.md`
- 开发者文档: `docs/dev/`

---

## 反馈

如果你在迁移过程中遇到问题或有改进建议：

1. 提交 GitHub Issue 并标记 `migration`
2. 在 issue 中描述：
   - 你尝试的旧命令
   - 预期的新命令
   - 遇到的问题
3. 我们会及时响应并更新本文档

---

## 时间线

| 日期 | 里程碑 |
|------|--------|
| 2026-01-08 | ✅ 新脚本系统发布 |
| 2026-02-01 | 旧脚本标记为废弃 |
| 2026-04-01 | 旧脚本移至 `deprecated/` |
| 2026-07-01 | 完全移除旧脚本 |

**当前阶段**: 新旧脚本共存期，推荐使用新脚本

---

## 检查清单

迁移完成后，确认以下项目：

- [ ] 所有 CI/CD 管道使用新脚本
- [ ] 文档中更新了脚本引用
- [ ] 团队成员了解新命令
- [ ] 自定义脚本已迁移
- [ ] 测试通过
- [ ] 旧脚本备份（如有自定义修改）

完成后，你已经成功迁移到新的脚本系统！🎉
