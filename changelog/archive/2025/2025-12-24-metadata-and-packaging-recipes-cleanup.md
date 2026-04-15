# 仓库元信息更新与打包配方清理（2025-12-24）

## 背景
- 仓库内存在 `your-org/you@example.com` 等占位符信息，容易误导使用者与下游工具（Conan/CMake/示例文档）。
- `dist/homebrew` 与 `dist/conda` 当前为占位配方，未处于可用状态；在不计划维护对应发布渠道时，保留它们会增加维护负担。

## 本次变更
- 元信息对齐
  - 更新 `conanfile.py`
    - `author`：`LessUp <jiashuai.mail@gmail.com>`
    - `url`：`https://github.com/LessUp/FastQTools`
  - 更新 `CMakeLists.txt`
    - `HOMEPAGE_URL`：`https://github.com/LessUp/FastQTools`
  - 更新 `examples/basic_usage/README.md`
    - 示例下载链接与 Issue 链接指向 `LessUp/FastQTools`
    - 修正文档中不存在的用户指南路径，改为仓库现有 `docs/user/usage.md`

- 打包/发布配方清理
  - 移除 `dist/homebrew/fastqtools.rb`
  - 移除 `dist/conda/meta.yaml`
  - 移除 `config/dependencies/conandata.yml`

## 影响范围
- 仅影响仓库元信息与发布配方文件，不影响核心功能代码。
- 如果未来需要支持 Homebrew/Conda 发布，可再引入对应配方并补齐版本 URL/sha256/维护者信息。
