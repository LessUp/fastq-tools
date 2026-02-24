---
title: "/new-module"
description: "创建新模块（含头文件、源文件、CMakeLists、单元测试骨架），遵循项目架构规范。"
auto_execution_mode: 1
---

## 适用场景
- 新增功能模块（如新的子命令、新的处理器、新的统计器）
- 需要一次性生成符合项目规范的文件骨架

## 工作流步骤
1. **确认模块信息**
   - 模块名称（snake_case，如 `quality_trimmer`）
   - 所属命名空间（如 `fq::processing`、`fq::io`、`fq::statistic`）
   - 模块类型：接口+实现 / 纯头文件 / CLI 子命令

2. **创建头文件**
   - 路径：`include/fqtools/<namespace>/<module_name>.h`
   - 模板：
     ```cpp
     #pragma once
     
     namespace fq::<namespace> {
     
     /// @brief <模块简要描述>
     class <ClassName> {
     public:
         <ClassName>() = default;
         ~<ClassName>() = default;
     };
     
     } // namespace fq::<namespace>
     ```
   - 命名规范：类名 PascalCase，方法 camelCase，私有成员 camelCase + `_` 后缀

3. **创建源文件**
   - 路径：`src/<namespace>/<module_name>.cpp`
   - 包含对应头文件：`#include <fqtools/<namespace>/<module_name>.h>`
   - 在 `src/<namespace>/CMakeLists.txt` 中添加源文件

4. **创建单元测试**
   - 路径：`tests/unit/<namespace>/test_<module_name>.cpp`
   - 模板：
     ```cpp
     #include <gtest/gtest.h>
     #include <fqtools/<namespace>/<module_name>.h>
     
     namespace fq::<namespace>::test {
     
     class <ClassName>Test : public ::testing::Test {
     protected:
         void SetUp() override {}
         void TearDown() override {}
     };
     
     TEST_F(<ClassName>Test, BasicConstruction) {
         // TODO: 实现测试
     }
     
     } // namespace fq::<namespace>::test
     ```
   - 在 `tests/unit/<namespace>/CMakeLists.txt` 中注册（如果存在）
   - 或在 `tests/unit/CMakeLists.txt` 中添加

5. **验证**
   // turbo
   - `./scripts/core/build -c clang -t Debug`
   - `./scripts/core/test -c clang -t Debug -f "*<ClassName>*"`

6. **收尾**
   - `./scripts/core/lint format-check`
   - 更新 `changelog/` 记录新模块
