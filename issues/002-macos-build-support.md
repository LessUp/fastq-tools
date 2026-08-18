---
id: 002
title: macOS 构建支持
status: closed
labels: [enhancement]
created: 2026-07-28
closed: 2026-08-18
---

## 现象

`scripts/core/install-deps` 在 macOS 上直接报错退出（"macOS 支持开发中"），仅支持 Ubuntu/Debian 系 Linux。macOS 用户只能手动安装依赖（Homebrew + Conan）并手动构建。

影响：macOS 开发者无法开箱即用；此前文档曾声称"macOS 原生支持"，与脚本行为矛盾（文档已更正为"脚本支持开发中（可手动构建）"）。这是已知缺口，不是路线图承诺。

## 根因

脚本把系统检查写成 Linux/apt 单路径，没有按 OS 选择包管理器，因此 macOS 只能硬拒绝。

## 处理

`install-deps` 在 macOS 上走 Homebrew：`cmake`/`ninja`/`llvm`（`--all` 另加 `ccache`），跳过 apt/dpkg。Conan 仍需按 getting-started 手动安装。文档改为“可用 install-deps 后构建”。

## 复盘

跨平台脚本先分支包管理器，再谈编译器细节。C++ 依赖继续交给 Conan，系统脚本只装工具链，避免两套包树。
