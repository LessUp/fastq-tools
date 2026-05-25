---
title: "/update-deps"
description: "更新 Conan 依赖并验证构建，确保依赖升级不引入回归。"
auto_execution_mode: 1
---

## 适用场景
- 定期依赖升级（安全补丁、新特性）
- CI 报依赖冲突或构建失败
- 新增第三方库

## 工作流步骤
1. **查看当前依赖版本**
   // turbo
   - `cat config/dependencies/conanfile.py`
   - `cat conanfile.py`（根目录的 Conan 配置）

2. **更新目标依赖**
   - 编辑 `config/dependencies/conanfile.py` 中对应包的版本号
   - 如果是新增依赖，同时更新 `conanfile.py` 和 `CMakeLists.txt`

3. **安装依赖并验证**
   - `./scripts/core/build -c clang -t Debug --clean`
   - 如果 Conan install 失败，检查版本兼容性或 `conan search <pkg>` 查找可用版本

4. **运行测试确认无回归**
   - `./scripts/core/test -c clang -t Debug`
   - 如有失败，定位是依赖 API 变更还是自身代码问题

5. **Release 构建验证**
   - `./scripts/core/build -c clang -t Release`
   - `./scripts/core/test -c clang -t Release`

6. **收尾**
   - `./scripts/core/lint format-check`
   - 如需记录依赖变更历史，更新根目录 `CHANGELOG.md`，不要重新创建 `changelog/`
