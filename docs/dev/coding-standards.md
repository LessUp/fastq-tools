# 编码规范

## 风格来源

- 本仓编码风格与工具配置（.clang-format、.clang-tidy、.editorconfig、.git-blame-ignore-revs）**借鉴并适配**自 MongoDB 仓库，旨在建立一套工程化且易于维护的 C++ 开发规范。
- **注意**：本项目并非 MongoDB 官方仓库规范的完全一致实现，而是在其基础上结合现代 C++20 特性（如尾随返回类型）和本项目需求进行的剪裁与优化。

## 基本规则

- **语言标准**: C++20
- **编译器**: GCC 11+, Clang 12+
- **格式化**: 使用 `.clang-format` 自动格式化（基于 LLVM 风格，4 空格缩进，列宽 100）

### 手动统一命令

- 全量格式化：`./scripts/lint.sh format`
- 只检查格式：`./scripts/lint.sh format-check`

## 命名约定

| 元素 | 风格 | 示例 |
|------|------|------|
| 代码文件/目录 | `snake_case` | `file_parser.h`, `src/utils/` |
| 文档 | `kebab-case` | `docs/dev/git-guidelines.md` |
| 脚本 | `snake_case` | `scripts/install_deps.sh` |
| 类/结构体 | `PascalCase` | `class FastQReader;` |
| 函数/方法 | `camelCase` | `void processBatch();` |
| 变量（局部变量/参数） | `camelCase` | `int readCount = 0;` |
| 类成员变量 | `camelCase` 并以 **尾部下划线** 结尾（例如：`member_`, `impl_`） | `filePath_` |
| 常量 | `kConstantName` | `constexpr int kMaxReads = 1000;` |
| 命名空间 | `snake_case` | `namespace fq::utils` |

> 文档采用 `kebab-case` 命名，脚本采用 `snake_case` 命名。

### 命名例外（仓库约定）

以下文件名为行业惯例/工具约定，不纳入 `snake_case` 约束：

- `README.md` / `CHANGELOG.md` / `LICENSE` / `CODE_OF_CONDUCT.md` / `CONTRIBUTING.md` / `SECURITY.md`
- `CMakeLists.txt` / `*.cmake` / `*.cmake.in`
- `Dockerfile` / `docker-compose.yml`
- `conanfile.py` / `package.json`

以下为生成物目录/文件，默认不参与命名规范检查：

- `build-*` / `cmake-build-*` / `_cmake_install_prefix/` / `_output/`

## 代码组织

### 目录结构

```
include/fqtools/     # 公共 API 头文件
src/                 # 内部实现代码
src/cli/             # 命令行入口
src/cli/commands/    # 子命令实现
src/processing/      # 处理逻辑 (Mutators, Predicates)
src/statistics/      # 统计分析逻辑
src/io/              # 输入输出处理
```

### 包含顺序

1. C++ 标准库
2. 第三方库
3. 本项目其他模块

### 接口设计

- 头文件只包含声明
- 实现细节放在源文件中
- 使用 `#pragma once` 防止重复包含

## 现代 C++ 实践

### 资源管理

```cpp
// 优先使用智能指针
std::unique_ptr<Processor> processor = create_processor();
std::shared_ptr<Config> config = std::make_shared<Config>();

// 禁止裸露的 new/delete
```

### 函数声明

```cpp
// 统一使用尾随返回类型
auto processBatch(const Batch& batch) -> Result;
```

### 错误处理

```cpp
// 优先使用异常
if (!is_valid(input)) {
    throw InvalidInputError("Invalid input file");
}
```

### 字符串处理

```cpp
// 优先使用 string_view 作为只读参数
auto process_sequence(std::string_view sequence) -> bool;
```

## CLI 规范

### 命令命名

- 子命令: 简短动词或名词 (`stat`, `filter`)
- 命令类: `PascalCase` + `Command` 后缀 (`StatCommand`)

### 选项格式

- 长选项: `kebab-case` (`--input`, `--min-quality`)
- 短选项: 单字母 (`-i`, `-o`, `-t`)
- 布尔开关: 正向选项，必要时提供 `--no-*`

### 全局选项

- `-v, --verbose`: 详细日志
- `-q, --quiet`: 仅错误输出
- `--log-level=LEVEL`: 显式设置日志级别

## 工具脚本

### 脚本规范

```bash
#!/bin/bash
set -euo pipefail  # 严格模式

# 函数命名: snake_case
print_info() {
    echo "INFO: $*"
}

# 变量引用: 双引号
run_build "$compiler" "$build_type"
```

### 统一工具

- `lint.sh`: 代码质量检查 (clang-tidy + clang-format)
- `build.sh`: 构建管理
- `test.sh`: 测试运行

## 文档要求

### API 文档

```cpp
/**
 * @brief 处理 FASTQ 批次数据
 * @param batch 要处理的批次数据
 * @return 处理结果
 * @throws ProcessingError 处理失败时抛出
 */
auto processBatch(const Batch& batch) -> Result;
```

### 代码注释

- 对复杂逻辑添加必要注释
- 避免显而易见的注释
- 保持注释与代码同步
