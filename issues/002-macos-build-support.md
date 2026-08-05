---
id: 002
title: macOS 构建支持
status: open
labels: [enhancement]
created: 2026-07-28
closed:
---

## 现象

`scripts/core/install-deps` 在 macOS 上直接报错退出（"macOS 支持开发中"），仅支持 Ubuntu/Debian 系 Linux。macOS 用户只能手动安装依赖（Homebrew + Conan）并手动构建。

影响：macOS 开发者无法开箱即用；此前文档曾声称"macOS 原生支持"，与脚本行为矛盾（文档已更正为"脚本支持开发中（可手动构建）"）。这是已知缺口，不是路线图承诺。

## 根因

（关闭时填写）

## 处理

建议方向：提供 Homebrew 依赖路径（cmake、ninja、llvm、tbb、zlib-ng、conan），在本地或 macOS CI runner 上验证构建与测试后，解除 `install-deps` 对 macOS 的硬拒绝；在此之前文档维持现状描述。

## 复盘

（关闭时填写）
