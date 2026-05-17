# 中文导读与白皮书叙事页重写

- 目标：将中文导读、白皮书与相关叙事页改写为批准的白皮书型信息架构与阅读语气。
- 主要改动：新增 `docs/tests/zh-page-matrix.test.mjs` 作为中文叙事页骨架测试；重写 `docs/zh/orientation/`、`docs/zh/whitepaper/`、`docs/zh/why-fastqtools/`、`docs/zh/architecture/`、`docs/zh/workflows/`、`docs/zh/knowledge-map/`、`docs/zh/performance/` 下相关页面，使其统一围绕导读、系统边界、执行模型与证据框架组织内容。
- 影响范围：中文白皮书叙事页、文档测试，以及同步生成的中英文发布说明索引。
- 验证方式：`cd docs && node --test tests/zh-page-matrix.test.mjs`；`cd docs && npm test`；`cd docs && npm run build`；`git --no-pager diff --check`。
