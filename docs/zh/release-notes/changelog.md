# 变更记录

本页面记录 FastQTools 每个版本的变更。

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- GitHub Release v3.1.0 with bilingual release notes

### Changed
- Optimized GitHub repository description for better visibility
- Updated GitHub topics: added `sequence-analysis`, `ngs-qc`, `fastq-processor`
- Restructured v3.1.0 release notes: English first, Chinese second format
- Verified all workflows are necessary and well-designed

### Fixed
- Fixed performance claims in README (1.7M reads/s instead of 1.7 billion)
- Fixed mkdocs.yml: removed non-existent custom_icons path
- Created missing migration notice documents for archive section

---

## 3.1.0 (2026-04-16)

### Release Highlights / 发布亮点

This release marks the first stable version with complete bilingual documentation support and professional project presentation.

本次发布标志着首个稳定版本，提供完整的双语文档支持和专业的项目展示。

### Added

**Core Features / 核心功能**
- `stat` command: FASTQ file statistical analysis with base composition, quality distribution, GC content
- `filter` command: Read filtering and quality trimming with multi-dimensional criteria support

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

## 3.0.0 (2025-07-31)

### Added
- Initial core functionality implementation
- CMake build system
- Conan dependency management

### Changed
- Reverted from C++20 modules to traditional header file structure
- Modular code architecture

---

## 2.0.0 (2024-07-29)

### Added
- Initial release
- FastQ file statistical analysis functionality
- Multi-threaded processing support
- Compressed file format support
- Command-line interface

---
