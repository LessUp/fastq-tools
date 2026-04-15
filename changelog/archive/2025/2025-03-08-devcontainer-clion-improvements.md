# DevContainer 改进：CLion 支持 + 脚本增强

**日期**: 2025-03-08
**类型**: feat(devcontainer)
**范围**: .devcontainer

## 变更内容

### 新增

- **CLion 专用配置** (`devcontainer.clion.json`)：为 JetBrains CLion 提供专用的
  devcontainer 配置，解决 CLion 与 VS Code 的兼容性差异（路径解析、initializeCommand
  支持等）

### 改进

- **container-setup.sh**：添加空命令 `""` 的独立处理 case，区分"无参数"和"未知命令"，
  改善错误提示体验
- **host-prepare.sh**：添加 SSH agent 可用性检查，在宿主机准备阶段提醒用户启动
  SSH agent 以支持 Git SSH 认证
- **README.md**：
  - 添加 CLion 使用指南（快速开始方式三）
  - 添加文件说明中的 `devcontainer.clion.json` 条目
  - 添加便捷构建命令参考
  - 添加"扩展安装缓慢"故障排除
  - 添加"切换配置"说明章节
  - 开发工具列表改为表格格式

## 借鉴来源

参考 fq-compressor 项目的 `.devcontainer` 配置改进。
