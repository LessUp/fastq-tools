---
title: "/release-package"
description: "生成 FastQTools 可分发包（复用 scripts/package_release.sh），产物输出到 dist/。"
auto_execution_mode: 1
---

## 适用场景
- 你希望生成一个可拷贝给他人使用的 `tar.gz` 包

## 工作流步骤
1. **安全前置**
   // turbo
   - `git status -sb`

2. **确认版本号策略**
- 你可以传入版本号：`./scripts/package_release.sh 3.1.0`
- 不传则默认为 `dev`：`./scripts/package_release.sh`

3. **执行打包**
- `./scripts/package_release.sh <version>`

4. **产物位置**
- 输出：`dist/fastqtools-v<version>-linux-x86_64.tar.gz`

## 注意事项
- 该脚本会在 `dist/` 下创建临时目录并在结束后清理
- 该脚本依赖 Release 构建；若环境缺依赖，请先运行 `./scripts/install_deps.sh`
