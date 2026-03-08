# fix(ci): 修复 CI 中 Clang + libc++ 链接失败问题

## 问题

在 GitHub Actions CI 中，使用 Clang 21 + libc++ 构建时，Conan 构建依赖（如 gtest）
链接阶段失败：

```
/usr/bin/ld: cannot find -lc++: No such file or directory
```

根因：`libc++-21-dev` 将库安装在 `/usr/lib/llvm-21/lib/`，但系统 GNU linker
(`/usr/bin/ld`) 的默认搜索路径不包含此目录。之前尝试的 `LIBRARY_PATH` 环境变量
方案无法传递到 Conan 子进程中。

## 修复

- **`config/conan/profile-clang`**: 添加 `tools.build:exelinkflags` 和
  `tools.build:sharedlinkflags` 指定 `-fuse-ld=lld`，使用 LLVM 的 lld 链接器
- **`config/dependencies/conanfile.py`**: 在 `generate()` 中为 Clang + libc++
  场景追加 `CMAKE_EXE_LINKER_FLAGS` 和 `CMAKE_SHARED_LINKER_FLAGS`
- **`.github/workflows/ci.yml`**: 所有安装 lld-21 的步骤添加
  `update-alternatives --install /usr/bin/ld.lld ld.lld /usr/bin/ld.lld-21 100`，
  替换无效的 `LIBRARY_PATH` 方案

## 影响文件

- `config/conan/profile-clang`
- `config/dependencies/conanfile.py`
- `.github/workflows/ci.yml`
