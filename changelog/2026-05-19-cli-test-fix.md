# 2026-05-19 CLI 单元测试修复

- 问题：`test_cli` 中的 `FilterPlanTest.BuildsAdapterAndPolyXTailMutatorsFromRepeatableOptions` 测试失败
- 原因：测试用例字符串 `"ACGTTTTT"` 实际有 5 个 T（不是 4 个），导致与测试期望不符
- 修复：将输入字符串改为 `"ACGTAAAAA"`，使用不同碱基作为尾部，避免与前面的 T 混淆
- 验证：所有单元测试通过
