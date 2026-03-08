# 修复 CI 检查失败

**日期**: 2025-03-08
**类型**: fix(ci)

## 变更内容

### Format Check 修复
- 使用 clang-format-21 格式化所有源文件，修复尾置返回类型换行风格等格式问题

### Static Analysis 修复
- 修复 `scripts/core/lint` 脚本中 `set -e` 导致 `run_xxx` 返回非零时脚本直接退出的问题
- 将 `run_xxx; RESULT=$?` 模式改为 `run_xxx && RESULT=0 || RESULT=$?`

### Clang 构建修复
- 将 CI 使用的 LLVM 版本从 21（不稳定开发快照）降级到 20（稳定版）
- 安装 `libc++-dev` 和 `libc++abi-dev` 包，设置正确的头文件路径
- 在 `conan install` 中为 clang 构建添加 `-pr:h config/conan/profile-clang`
- 更新 Conan clang profile、CMakePresets.json、AGENTS.md 中的版本引用

### Docker Build 修复
- 修复 `Dockerfile.prod` 和 `Dockerfile.deploy` 中 GLIBCXX 版本不匹配问题
- 从构建阶段收集 GCC 15 的 libstdc++ 和 libgcc_s 运行时库
- 在生产阶段复制这些库并运行 ldconfig
