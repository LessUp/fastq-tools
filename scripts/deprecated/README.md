# ⚠️ 废弃脚本目录

**此目录中的所有脚本已被废弃，仅供兼容性保留。**

## 警告

这些脚本将在 **2026-07-01** 完全移除。请立即迁移到新脚本。

## 替代方案

| 废弃脚本 | 新脚本 | 迁移命令 |
|----------|--------|----------|
| `build.sh` | `../core/build` | `scripts/core/build` |
| `build-dev.sh` | `../core/build --dev` | `scripts/core/build --dev` |
| `test.sh` | `../core/test` | `scripts/core/test` |
| `lint.sh` | `../core/lint` | `scripts/core/lint` |
| `install_deps.sh` | `../core/install-deps` | `scripts/core/install-deps` |
| `install_runtime.sh` | `../core/install-deps --runtime` | `scripts/core/install-deps --runtime` |

## 迁移指南

请参阅项目根目录的 `MIGRATION.md` 文件获取详细的迁移指南。

## 时间线

- **2026-01-08**: 脚本移至此目录
- **2026-02-01**: 添加运行时警告
- **2026-04-01**: 停止功能更新
- **2026-07-01**: 完全删除

## 为什么废弃？

新脚本系统提供：
- ✅ 统一的命令接口
- ✅ 更好的错误处理
- ✅ 完善的文档
- ✅ 清晰的目录结构
- ✅ 公共函数库避免重复

## 需要帮助？

- 查看新脚本文档: `scripts/README.md`
- 迁移指南: `MIGRATION.md`
- 架构文档: `scripts/ARCHITECTURE.md`
- 提交问题: GitHub Issues
