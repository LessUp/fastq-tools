# 2026-03-24 构建链路与测试回归加固

## 目标
修复项目当前存在的默认开发构建失效、脚本参数语义不一致、E2E 基线仍容忍失败，以及处理模块缺少关键回归保护等问题。

## 变更内容

### 1. 修复默认 Clang 开发构建链路
- 更新 `scripts/core/build`，在未显式指定时自动探测本机 `clang` / `g++` 主版本，并将版本传给 Conan。
- 新增 `--conan-compiler-version` 参数，允许在特殊环境下显式覆盖 Conan 编译器版本。
- 解决本地 Conan 版本表较旧时，`profile-clang` 中固定 `compiler.version=21` 导致 `./scripts/core/build --dev` 直接失败的问题。

### 2. 修复测试脚本参数语义冲突
- 调整 `scripts/core/test`：
  - `--type` 现在明确用于选择测试类型（`all | unit | integration | e2e`）。
  - 构建类型改为 `--build-type` / `-t`。
- 同步修正帮助信息与默认构建目录说明，避免脚本文档和真实行为不一致。

### 3. 将 E2E 从“容忍失败基线”改为真实回归测试
- 更新 `tests/e2e/test_cli.sh`：
  - `filter` / `stat` 失败时直接报错，不再保留“known issue”绕过逻辑。
  - 新增 `trim-quality` 实际裁剪效果检查。
- 更新 `tests/e2e/test_advanced_cli.py`：
  - 新增 `trim-quality` 行为回归测试。

### 4. 补强处理模块的单元回归覆盖
- 扩充 `tests/unit/processing/test_pipeline_smoke.cpp`，覆盖：
  - `QualityTrimmer` 的双端、5'、3' 裁剪与最小长度限制。
  - Phred+64 编码分支。
  - `MinQualityPredicate`、长度谓词、`MaxNRatioPredicate` 的关键边界行为。
  - `AdapterTrimmer`、`LengthTrimmer` 的基本语义。
  - `FastqRecord` / `FastqBatch` 的基础一致性与 remainder 行为。

### 5. 清理构建噪音
- 从 `config/conan/profile-clang` 中移除 Clang 18 不识别的 `-Wno-character-conversion`。
- 清理 `src/io/fastq_reader.cpp` 中未使用的 `gzerror()` 临时变量，消除无意义编译警告。

### 6. 清理 CI 与文档中的高风险不一致
- 移除 `quality.yml` 中失败后自动格式化并直接 `git push` 的行为，避免 CI 在未经确认的情况下修改分支历史。
- 将 `tests/unit/common/test_common.cpp` 中对已废弃 `Logger` 的测试替换为 `IDGenerator` 回归测试，减少默认构建中的 deprecated 噪音。
- 修正 API 文档中的 CMake 集成示例，使其与当前导出目标和 consumer 验证保持一致。
- 更新 `config/README.md` 中对 Clang profile 的描述，避免继续误导为固定版本才能使用。

### 7. 已知仍待后续处理的范围
- 仓库中仍有较多历史文档、archive/spec/changelog 文本引用旧脚本名、旧构建目录或固定 Clang 21 叙述；这些不会影响当前构建与测试，但仍值得后续做一次系统清理。
- `tests/cmake_package_consumer/` 已接入常规 CTest 验证流程，通过安装到临时前缀并使用对应 Conan toolchain 独立配置/构建 consumer 来校验导出包可用性。
- 修复安装导出目标的 `INTERFACE_INCLUDE_DIRECTORIES` 污染问题，避免导出结果出现错误的 `.../COMPONENT/Development` 路径。










































































































































n/a

## 验证结果
- `./scripts/core/build -c gcc -t Debug`
- `./scripts/core/test -b build/gcc-debug --type all`
- `./scripts/core/build --dev`
- `ctest --test-dir build/clang-debug --output-on-failure`

以上验证均已通过。
