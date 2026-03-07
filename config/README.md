# Configuration Directory

This directory contains all configuration files for the FastQTools project, organized by purpose.

## Directory Structure

```
config/
├── conan/              # Conan compiler profiles
│   ├── profile-clang   # Clang 21 + libc++
│   └── profile-gcc     # GCC 15 + libstdc++
├── cppcheck/           # Cppcheck static analysis
│   ├── cppcheck.cfg    # Project configuration (paths, defines, platform)
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
conan install config/dependencies/ -pr:h config/conan/profile-clang --build=missing
conan install config/dependencies/ -pr:h config/conan/profile-gcc   --build=missing
```

### Cppcheck

```bash
cppcheck --project=config/cppcheck/cppcheck.cfg \
         --suppressions-list=config/cppcheck/suppressions.txt
```

### Include-What-You-Use

```bash
include-what-you-use -Xiwyu --mapping_file=config/iwyu/mappings.imp
```

### Sanitizers

```bash
# Load options as colon-separated string
export ASAN_OPTIONS=$(grep -v '^#' config/sanitizers/asan.options | grep -v '^$' | tr '\n' ':')
```

### Valgrind

```bash
valgrind --suppressions=config/valgrind/suppressions.supp ./FastQTools
```

> **Tip**: The `scripts/core/build`, `scripts/core/test`, and `scripts/core/lint` scripts
> automatically locate and use the correct config files — no manual path setup needed.
