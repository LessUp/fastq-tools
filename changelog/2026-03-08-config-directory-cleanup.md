# config 目录检查与修复

**日期**: 2026-03-08
**类型**: fix(build), chore

## 变更内容

### 修复 Conan Profile
- `profile-clang`: 编译器版本 20→21，与 AGENTS.md 和 CI 实际使用一致
- `profile-clang`: 注释说明 libc++ 仅本地开发使用，CI 使用 libstdc++

### 修复 conanfile.py (config/dependencies/ 和根目录)
- `exports_sources`: 移除不存在的 `app/*`/`third_party/*`，添加 `include/*`/`tests/*`/`config/*`/`tools/*`

### 更新 README.md
- profile-clang 描述注明 CI 使用 libstdc++
