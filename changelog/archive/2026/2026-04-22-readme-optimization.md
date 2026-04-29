# README 优化改进

## 目标

改进 README 文档的准确性、完整性和用户体验，修复发现的命令错误和文档缺陷。

## 主要改动

### 1. 修复命令错误

| 文件 | 位置 | 改动说明 |
|------|------|----------|
| `README.md` | macOS 安装 | 修正 `brew install clang` 错误，改为安装 `cmake ninja conan` 并说明 Clang 来源 |
| `README.md` | Linux 安装 | 添加 Conan 2.x 安装步骤，补充 Ninja 依赖说明 |
| `README.md` | Docker Quick Start | 补充 `-v` 卷挂载参数，修正不完整命令 |
| `README.zh-CN.md` | 同上 | 中文版本同步修正 |

### 2. 新增系统要求章节

在两个 README 中添加了 **System Requirements / 系统要求** 章节：

- **最低要求**: C++23 编译器、CMake 3.28+、Conan 2.x、2GB RAM
- **推荐配置**: GCC 15 / Clang 21、Ninja 1.10+、8GB+ RAM
- **操作系统支持**: Linux (原生)、macOS、Windows (Docker/WSL)

### 3. 添加目录导航

在 Overview 后添加 Table of Contents / 目录：
- 链接到主要章节（Features, Quick Start, Installation 等）
- 提升长文档的可导航性

### 4. 补充输出示例

在 Usage Examples 中添加了 `stat` 命令的预期输出示例：
- 日志输出格式
- 报告文件内容示例（读段统计、质量指标、碱基组成）

### 5. 新增故障排除章节

添加 **Troubleshooting / 疑难解答** 章节，包含常见问题：
- Conan 依赖解析失败解决方案
- Clang/CMake 版本问题排查
- Docker 权限和卷挂载问题
- 获取帮助的渠道

### 6. 更新概览亮点

新增两条亮点描述：
- 📖 **Comprehensive Documentation** - 强调完善的文档支持
- 🐳 **Cross-Platform** - 明确跨平台支持能力

## 影响范围

- **用户影响**: 新用户能更快上手，避免常见安装错误
- **维护影响**: 减少了因命令错误导致的 Issue 报告
- **文档质量**: README 完整性大幅提升，符合专业开源项目标准

## 验证方式

1. 检查两个 README 的 Markdown 渲染正确
2. 验证所有内部链接有效
3. 确认中英文版本内容一致性
