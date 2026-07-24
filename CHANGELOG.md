# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

This root changelog is the maintained project history. Older granular work logs from the retired `changelog/` tree have been folded into the yearly and release summaries below.

---

## [Unreleased]

### Changed
- Consolidated the maintained repository history into this file and retired the legacy `changelog/` directory.
- Retired docs-site changelog publication in favor of GitHub Releases plus the root `CHANGELOG.md`.

### Architecture and runtime
- Continued the post-v3.1.0 architecture cleanup with injectable I/O abstractions, shared CLI/runtime configuration helpers, centralized statistics reporting, and clearer execution-runtime seams.
- Reduced shallow wrappers and false seams, tightened error-handling boundaries, and fixed follow-up CI workflow regressions plus a CLI test regression during the refactor cycle.

### Documentation and project presentation
- Rebuilt the documentation whitepaper surface with stronger information architecture, bilingual narrative pages, theme-aware diagrams, research and algorithms routing fixes, and responsive homepage/layout follow-ups.
- Cleaned publication rules so internal asset README files are not treated as user-facing documentation.

---

## [4.0.0] - 2026-07-17

FastQTools v4 is an intentional breaking release that narrows the public surface and makes runtime/resource contracts explicit.

### Breaking
- Replaced `createProcessingPipeline()` and `createStatisticCalculator()` with move-only PIMPL classes `fq::processing::Pipeline` and `fq::statistics::Calculator`.
- Renamed the statistics namespace from `fq::statistic` to `fq::statistics`.
- Added `IWriter::finish()`; `write()` only accepts data, while `finish()` reports flush, compression-close, and publication errors.
- Removed config, ErrorHandler, legacy Logger, public object pools, and the Taskflow backend. No v3 compatibility layer is provided.
- Consolidated the installed CMake consumer target to `FastQTools::FastQTools`; CLI-only dependencies remain private.

### Correctness and runtime
- Filter order is fixed to adapter trim → poly-G/poly-X trim → quality trim → predicates; all predicates use the final trimmed read.
- Default file output is staged in a same-directory temporary file and atomically renamed after a successful `finish()`; failed writes preserve the previous target.
- Memory profiles account for batch storage, record vectors, reader remainder, and writer/zlib buffers. Limits below one runtime working set raise `ConfigurationError`.
- Sequential and oneTBB paths share deterministic output/statistics contracts; CLI is the single exception/logging/exit-code boundary.

### Performance and verification
- Rebuilt production benchmarks around a fixed seed=42, 1M×150 bp dataset, real Reader/Writer/Pipeline/StatisticWorker code, and five repetitions with median/CV summaries.
- Writer measurements cover plain and gzip-1/6/9, each with single and batch APIs; unstable WSL2 clock data did not meet the evidence threshold for speculative Writer/statistics optimization.
- Manual CI runs format, static analysis, GCC/Clang, ASan, TSan, UBSan, and coverage jobs when dispatched from GitHub Actions.

---

## [3.2.0] - 2026-04-30

### Added
- Advanced preprocessing capabilities called out by the `v3.2.0` release tag, including quality trimming, poly-tail trimming, adapter trimming, and related CLI options such as `--adapter-seq`, `--trim-poly-g`, and `--trim-poly-x`.
- QC signature reporting called out by the `v3.2.0` release tag, including optional signature sidecar output, duplicate estimation, and head-kmer statistics.

### Changed
- Repository history at the `v3.2.0` tag shows accompanying cleanup around CI simplification plus documentation, GitHub Pages, and AI-governance reorganization.

---

## [3.1.0] - 2026-04-16

### Added
- First stable bilingual documentation release across the README, documentation site, and release materials.
- Production-ready FASTQ processing commands: `stat` for statistical analysis and `filter` for read filtering and quality trimming.
- High-performance processing foundations including TBB `parallel_pipeline`, zero-copy FASTQ record views, and libdeflate-backed compression support.
- Broader quality tooling with unit/integration/e2e tests, Google Benchmark, fuzzing support, sanitizers, Valgrind checks, Docker images, and DevContainer setup.

### Changed
- Standardized the project on C++23, modern CMake presets, and Conan 2.x dependency management.
- Completed the public API/interface split, unified logging and error handling, and tightened naming/documentation conventions across the codebase.
- Reworked the documentation structure, bilingual navigation, and release presentation for a stable public release.

### Fixed
- Resolved documentation workflow issues around i18n, static-site configuration, and release-note consistency.
- Addressed code quality findings affecting CLI logging consistency and public API annotations.

---

## 2026 Development Summary

- **Toolchain and quality**: unified GCC 15 / Clang 21 expectations, completed the C++23 migration, simplified selected dependency configuration (including header-only `fmt` usage), added benchmark and coverage improvements, and hardened build/test workflows.
- **Developer environment**: iterated on DevContainer and Docker support, remote/AI-assisted development integration, and local tooling guidance so contributors could build and validate the project more reliably.
- **Repository convergence**: cleaned repository structure, aligned baseline specifications with implementation reality, improved workflow reliability, and refined release/readme/docs presentation leading into and after v3.1.0.
- **Post-release maintenance**: May 2026 focused on architecture deepening and whitepaper/content refinement; the key outcomes of that work are reflected in the Unreleased section above.

---

## [3.0.0] - 2025-07-31

### Added
- Initial modular implementation of the core FASTQ processing functionality.
- Modern CMake-based build system and Conan-managed dependency setup.

### Changed
- Reverted from experimental C++20 modules to the traditional header/source layout used by the project today.
- Reorganized the codebase around clearer module boundaries for I/O, processing, statistics, and CLI concerns.

---

## 2025 Development Summary

- **Project bootstrap**: established the repository layout, build scripts, Docker/devcontainer setup, CI/CD foundations, and early documentation structure.
- **Core implementation**: built FASTQ I/O, processing pipelines, statistics analysis, and early high-performance refactors around the TBB-based architecture.
- **Quality and packaging**: expanded tests, multi-platform release automation, consumer/package verification, install fixes, and dependency cleanup.
- **Repository maturation**: completed major docs restructures, naming/style unification, public header relocation, script cleanup, and end-of-year configuration alignment that set up the 2026 stabilization work.

---

## [2.0.0] - 2024-07-29

### Added
- Initial public release of FastQTools.
- FASTQ statistical analysis, multi-threaded processing, compressed file support, and a command-line interface.
