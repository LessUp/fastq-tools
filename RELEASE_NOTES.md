# FastQTools v3.1.0 Release Notes

## 🎉 Release Highlights / 发布亮点

We are excited to announce FastQTools v3.1.0, featuring complete bilingual (Chinese & English) documentation support and professional project presentation!

我们很高兴宣布 FastQTools v3.1.0 发布，提供完整的双语（中英文）文档支持和专业的项目展示！

---

## ✨ What's New / 新特性

### 🌐 Internationalization / 国际化
- **Complete Bilingual Documentation / 完整双语文档**: All user-facing documentation now available in both Chinese and English
- **Professional README / 专业 README**: Optimized project presentation with badges and visual elements
- **MkDocs Bilingual Support / MkDocs 双语支持**: Full i18n support with language switching

### 📚 Documentation Overhaul / 文档重构
- **17 New English Documents / 17 个新英文文档**: Full coverage of API, user guides, and developer docs
- **Standardized Changelog / 标准化变更日志**: New template format and professional archive system
- **Enhanced Navigation / 增强导航**: Reorganized docs structure for better readability

### 🔧 Core Improvements / 核心改进
- Enhanced `[[nodiscard]]` attributes across public APIs
- Unified logging framework usage in CLI commands
- Code quality improvements and static analysis fixes

---

## 📊 Performance / 性能

| Metric | Value | Level |
|--------|-------|-------|
| FastQReader | 1696 MB/s | 🟢 Extreme |
| FastQWriter | 1.76 M reads/s | 🟢 Extreme |
| Filter Combined | 1.67 M reads/s | 🟢 Extreme |
| Stat Full | 302 MB/s | 🟡 High |

---

## 🚀 Quick Start / 快速开始

```bash
# Clone / 克隆
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools

# Install dependencies / 安装依赖
./scripts/core/install-deps

# Build / 构建
./scripts/core/build

# Run tests / 运行测试
./scripts/core/test
```

---

## 📦 Installation / 安装

### From Source / 从源码构建

```bash
# Development build / 开发构建
./scripts/core/build --dev

# Release build / 发布构建
./scripts/core/build --type Release

# With sanitizers / 带检测器
./scripts/core/build --sanitizer asan --dev
```

### Docker / Docker 部署

```bash
# Production / 生产环境
docker-compose -f docker/docker-compose.yml up -d prod

# Development / 开发环境
docker-compose -f docker/docker-compose.yml up -d dev
```

---

## 📖 Documentation / 文档

- **Full Documentation / 完整文档**: https://lessup.github.io/fastq-tools/
- **中文文档**: https://lessup.github.io/fastq-tools/zh/
- **English Docs**: https://lessup.github.io/fastq-tools/en/

### Key Documents / 重点文档

| Document | 中文 | English |
|----------|------|---------|
| Getting Started | [快速入门](https://lessup.github.io/fastq-tools/guide/getting-started/) | [Getting Started](https://lessup.github.io/fastq-tools/en/guide/getting-started/) |
| CLI Reference | [CLI 参考](https://lessup.github.io/fastq-tools/guide/cli-reference/) | [CLI Reference](https://lessup.github.io/fastq-tools/en/guide/cli-reference/) |
| API Docs | [API 文档](https://lessup.github.io/fastq-tools/api/overview/) | [API Docs](https://lessup.github.io/fastq-tools/en/api/overview/) |

---

## 🛠️ System Requirements / 系统要求

- **OS**: Ubuntu 20.04+ / Debian 11+ / Windows (WSL2)
- **Compiler**: GCC 15+ or Clang 21+
- **CMake**: 3.28+
- **Conan**: 2.x
- **Memory**: 4GB+ RAM recommended

---

## 🔄 Changes from v3.0.0 / 自 v3.0.0 的变更

### Added / 新增
- Complete bilingual documentation (17 new English docs)
- Professional changelog template system
- Enhanced README with badges and visual elements
- MkDocs i18n configuration improvements

### Changed / 变更
- Restructured docs navigation for bilingual support
- Updated coding standards documentation
- Improved error handling consistency

### Fixed / 修复
- Documentation workflow optimizations
- Code quality improvements

---

## 📋 Full Changelog / 完整变更日志

See [CHANGELOG.md](CHANGELOG.md) for detailed version history.

查看 [CHANGELOG.md](CHANGELOG.md) 了解详细版本历史。

---

## 🙏 Acknowledgments / 致谢

Special thanks to all contributors who helped make this release possible!

特别感谢所有为本次发布做出贡献的贡献者！

---

## 📜 License / 许可证

[MIT License](LICENSE) © 2025-2026 LessUp

---

<p align="center">
  <sub>Built with ❤️ by the LessUp team / 由 LessUp 团队用 ❤️ 打造</sub>
</p>
