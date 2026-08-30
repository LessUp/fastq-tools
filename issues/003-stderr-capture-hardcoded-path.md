---
id: 003
title: 单测 StderrCapture 硬编码 /tmp 路径导致段错误
status: closed
labels: [bug, test]
created: 2026-08-30
closed: 2026-08-30
---

## 现象

在本机（`/tmp/opencode/fqt-verify/` 目录不存在的环境）运行 `test_statistics` 时，
全部用例通过后进程以 SIGSEGV（退出码 139）崩溃；同时被捕获的 `[warn] No reads found`
日志直接泄漏到了控制台。

## 根因

`tests/unit/statistics/test_statistics_report.cpp` 的 `StderrCapture` 测试工具
硬编码 `/tmp/opencode/fqt-verify/stderr_cap.txt` 作为捕获文件（疑似作者本机目录），
且对 `open`/`fopen` 的失败没有任何处理：目录不存在时 `fopen` 返回空 `FILE*`，
`content()` 对其调用 `fgets` 直接段错误。该缺陷依赖环境，CI 机器恰好存在该目录时
不会暴露，属于典型的环境相关测试脆弱性。

## 处理

重写 `StderrCapture`：捕获文件改为系统临时目录下带 pid 序号的唯一文件，
`open` 失败立即抛 `std::runtime_error` 快速失败（gtest 会报告为用例失败而非崩溃），
`content()` 直接从已持有的 fd `pread` 读取，消除二次 `fopen` 的空指针路径，
析构时 `unlink` 清理。已通过"还原修复→复现 SIGSEGV→恢复修复→通过"的方式验证根因。

## 复盘

- 测试工具中对文件/目录的任何创建都必须检查失败并快速失败，绝不能带着
  无效句柄继续执行；对 `FILE*`/fd 的使用要么判空要么由类型系统保证非空。
- 测试基建不应依赖机器特定路径；临时文件一律走 `temp_directory_path()` + 唯一名。
- 环境相关的段错误（退出码 139）在排查时应优先怀疑测试工具自身，而非被测代码。
