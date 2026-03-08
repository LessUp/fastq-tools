# FastQTools 项目结构

## 目录布局

```
FastQTools/
├── include/fqtools/      # 公共 API 头文件（稳定接口）
├── src/                  # 实现源码
│   ├── cli/              # CLI 入口与命令分发
│   │   └── commands/     # 子命令实现（stat, filter）
│   ├── common/           # 公共工具
│   ├── config/           # 配置管理
│   ├── error/            # 错误类型与处理
│   ├── io/               # FASTQ 读写
│   ├── processing/       # 流水线、修剪器、过滤谓词
│   │   ├── mutators/     # 读段修改（修剪）
│   │   └── predicates/   # 过滤条件
│   └── statistics/       # 统计计算
├── tests/                # 测试套件
│   ├── unit/             # 单元测试（按模块组织）
│   ├── integration/      # 集成测试
│   └── e2e/              # 端到端 CLI 测试
├── scripts/              # 构建与开发脚本
│   ├── core/             # 核心脚本（build, test, lint）
│   ├── tools/            # 专用工具脚本
│   └── lib/              # Shell 公共函数库
├── tools/                # 开发工具
│   ├── benchmark/        # 性能基准测试
│   └── data/             # 测试数据文件
├── docs/                 # 文档
│   ├── guide/            # 用户指南
│   ├── dev/              # 开发者文档
│   ├── api/              # API 参考
│   ├── decisions/        # 架构决策记录
│   └── specs/            # 功能规格
├── config/               # 配置文件
│   ├── conan/            # Conan 编译器 profiles
│   ├── dependencies/     # Conan 依赖配置
│   ├── sanitizers/       # ASan/TSan/UBSan 运行时选项
│   ├── valgrind/         # Valgrind 抑制规则
│   ├── cppcheck/         # Cppcheck 配置
│   ├── coverage/         # 覆盖率阈值配置
│   └── iwyu/             # Include-What-You-Use 映射
└── cmake/                # 自定义 CMake 模块
```

## 关键文件

- `CMakeLists.txt` — 根构建配置
- `conanfile.py` — 依赖定义
- `.clang-format` — 代码格式化规则
- `.clang-tidy` — 静态分析配置

## 库模块（CMake targets）

- `fq_common` — 公共工具
- `fq_error` — 错误处理
- `fq_config` — 配置管理
- `fq_modern_io` — I/O 操作
- `fq_processing` — 处理流水线
- `fq_statistics` — 统计计算
- `fq_cli` — CLI 命令
- `fq_lib` — 聚合接口库

## 命名约定

| 元素 | 风格 | 示例 |
|---------|-------|---------|
| 源文件 | `snake_case` | `fastq_reader.cpp` |
| 头文件 | `snake_case` | `processing_pipeline.h` |
| 类/结构体 | `PascalCase` | `FastQReader` |
| 函数/方法 | `camelCase` | `processBatch()` |
| 成员变量 | `camelCase_` | `filePath_` |
| 常量 | `kPascalCase` | `kMaxReads` |
| 命名空间 | `snake_case` | `fq::utils` |
| 文档 | `kebab-case` | `coding-standards.md` |
| 脚本 | `kebab-case` | `install-deps` |

## 架构层次

1. **CLI 层** → 参数解析、命令分发
2. **命令层** → 业务逻辑编排
3. **核心库层** → 公共接口（`include/fqtools/`）
4. **实现层** → 内部模块（`src/`）
