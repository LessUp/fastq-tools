# Configuration Directory

This directory contains all configuration files for the FastQTools project, organized by purpose.

## Directory Structure

```
build-config/
├── conan/              # Conan compiler profiles
│   ├── profile-clang   # Clang + libc++（本地开发）
│   ├── profile-clang-ci# Clang + libstdc++（CI）
│   └── profile-gcc     # GCC 15 + libstdc++
├── cppcheck/           # Cppcheck static analysis
│   └── suppressions.txt# Known false-positive suppressions
├── dependencies/       # Conan dependency recipes
│   └── conanfile.py    # All third-party dependency declarations
├── coverage/           # Code coverage thresholds
│   └── thresholds.json # Line/function coverage minimums
├── iwyu/               # Include-What-You-Use
│   └── mappings.imp    # Private→public header mapping rules
├── sanitizers/         # Compiler sanitizer runtime options
│   ├── asan.options    # AddressSanitizer
│   ├── msan.options    # MemorySanitizer (Clang only)
│   ├── tsan.options    # ThreadSanitizer
│   └── ubsan.options   # UndefinedBehaviorSanitizer
└── valgrind/           # Valgrind dynamic analysis
    └── suppressions.supp # Third-party library false-positive suppressions
```

## Usage

### Conan Profiles

```bash
conan install build-config/dependencies/ -pr:h build-config/conan/profile-clang --build=missing
conan install build-config/dependencies/ -pr:h build-config/conan/profile-clang-ci --build=missing
conan install build-config/dependencies/ -pr:h build-config/conan/profile-gcc   --build=missing
```

### Cppcheck

```bash
# 使用编译数据库（推荐，先构建以生成 compile_commands.json）
cppcheck --project=build/clang-debug/compile_commands.json \
         --suppressions-list=build-config/cppcheck/suppressions.txt

# 或直接扫描源码目录
cppcheck --suppressions-list=build-config/cppcheck/suppressions.txt \
         src include
```

### Include-What-You-Use

```bash
include-what-you-use -Xiwyu --mapping_file=build-config/iwyu/mappings.imp
```

### Sanitizers

```bash
# Load options as colon-separated string
export ASAN_OPTIONS=$(grep -v '^#' build-config/sanitizers/asan.options | grep -v '^$' | tr '\n' ':')
```

### Valgrind

```bash
valgrind --suppressions=build-config/valgrind/suppressions.supp ./FastQTools
```

> **Tip**: The `scripts/core/build`, `scripts/core/test`, and `scripts/core/lint` scripts
> automatically locate and use the correct config files — no manual path setup needed.
