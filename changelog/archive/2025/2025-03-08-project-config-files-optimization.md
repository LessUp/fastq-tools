# 项目配置文件优化

**日期**: 2025-03-08
**类型**: chore
**范围**: 项目根配置文件

## 变更内容

### `.dockerignore`
- 统一注释风格为 `===== 分类 =====` 格式
- 补充缺失排除项：`.github/`、`.devcontainer/`、`examples/`、`tools/fuzz/`、`dist/`
- 排除构建不需要的项目元文件（`.editorconfig`、`.clang-format`、`LICENSE` 等）
- 补充 `*.bak`、`*.sarif` 临时文件
- 移除不再需要的 `!README.md`、`!CHANGELOG.md` 例外（Docker 构建不需要 .md 文件）

### `.editorconfig`
- 全局 `[*]` 设置 `charset = utf-8` 和默认缩进 `indent_style = space` / `indent_size = 4`
- 大幅精简：C++、Python、Shell、JSON、JS 等均继承全局默认，消除冗余
- 合并 `*.yml` 与 `*.yaml` 为统一规则
- 移除非标准属性 `cpp_indent_braces`（VS 专用，EditorConfig 规范不支持）
- 新增 `[Makefile]` 使用 tab 缩进

### `.git-blame-ignore-revs`
- 补充 `git config --local blame.ignoreRevsFile` 启用方式说明

### `.gitattributes`
- 为 C++ 文件添加 `diff=cpp` 驱动（改善 diff hunk header 可读性）
- 为脚本添加 `diff=bash`、`diff=python` 驱动
- 补充缺失文件类型：`*.py`、`*.js`、`*.txt`、`*.cfg`、`*.hxx`、`*.fastq`
- 补充缺失二进制类型：`*.svg`、`*.ico`、`*.woff`、`*.woff2`
- 统一中文注释风格

### `.gitignore`
- 补充 Python 缓存：`__pycache__/`、`*.pyc`、`*.egg-info/`
- 补充 `dist/` 发布目录
- 补充 `.cache/`、`.ccache/` 缓存目录
- 补充 `Testing/` CTest 输出目录
- 补充 `*.dSYM/` macOS 调试符号

### `.gitmessage.txt`
- 修复过时引用：`gulp, npm` → `cmake, conan`
- 修复过时引用：`Travis, Circle` → `GitHub Actions`
- 修复不存在文件引用：`walkthrough.md` → `CONTRIBUTING.md`
- 修正错别字：`勾子` → `钩子`

### `commitlint.config.js`
- 改善代码格式可读性（单行展开为多行）
- 添加 `scope-enum` 规则（warning 级别），列出项目模块作为推荐 scope
- 添加 `scope-empty` 规则允许无 scope 提交
