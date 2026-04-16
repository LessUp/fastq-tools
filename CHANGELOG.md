# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- 🌐 **Internationalization**: Complete bilingual documentation (Chinese & English) for all user-facing docs
- 📚 **Documentation**: Professional restructuring of docs/ directory with comprehensive bilingual support
- 📝 **Changelog**: New standardized changelog template and professional archiving system
- ✨ **Code Quality**: Enhanced `[[nodiscard]]` attributes across public APIs

### Changed
- 📖 **README**: International professional optimization with better visual presentation
- 🔧 **Docs Structure**: Reorganized navigation in mkdocs.yml for bilingual support
- 🎨 **Code Style**: Unified logging framework usage across CLI commands

### Fixed
- 🐛 **Build**: Minor documentation workflow optimizations

---

## [3.1.0] - 2026-04-16

### Release Highlights / 发布亮点

This release marks the first stable version with complete bilingual documentation support and professional project presentation.

本次发布标志着首个稳定版本，提供完整的双语文档支持和专业的项目展示。

### Added

**Core Features / 核心功能**
- ✅ `stat` command: FASTQ file statistical analysis with base composition, quality distribution, GC content
- ✅ `filter` command: Read filtering and quality trimming with multi-dimensional criteria support

**Performance / 性能优化**
- Intel TBB `parallel_pipeline` based parallel processing
- `FastqRecord` zero-copy I/O using `string_view`
- libdeflate high-performance compression/decompression

**Development Tools / 开发工具**
- Comprehensive unit/integration/e2e test coverage
- Google Benchmark performance testing
- LibFuzzer fuzzing support
- ASan/TSan/UBSan sanitizers integration
- Valgrind memory checking configuration

**Build System / 构建系统**
- CMake 3.28+ modern configuration
- Conan 2.x dependency management
- GCC 15 / Clang 21 compiler support
- CMake Presets multi-configuration support

**Docker Support / Docker 支持**
- Development environment image (gcc:15.2-bookworm)
- Production deployment image (debian:bookworm-slim)
- VS Code DevContainer configuration

### Changed

- Language standard upgraded from C++20 to C++23
- Implemented interface-implementation separation modular architecture
- Unified error handling and logging system
- MongoDB-style naming conventions

---

## [3.0.0] - 2025-07-31

### Added
- Initial core functionality implementation
- CMake build system
- Conan dependency management

### Changed
- Reverted from C++20 modules to traditional header file structure
- Modular code architecture

---

## [2.0.0] - 2024-07-29

### Added
- Initial release
- FastQ file statistical analysis functionality
- Multi-threaded processing support
- Compressed file format support
- Command-line interface

---

## Change Log Index / 变更记录索引

| 年份 / Year | 汇总 / Summary | 详情 / Details |
|-------------|----------------|----------------|
| 2026 | [Summary / 汇总](changelog/2026-summary.md) | [Archive / 归档](changelog/archive/2026/) |
| 2025 | [Summary / 汇总](changelog/2025-summary.md) | [Archive / 归档](changelog/archive/2025/) |

**Template / 模板**: [TEMPLATE.md](changelog/TEMPLATE.md)
