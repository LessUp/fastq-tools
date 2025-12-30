# FastQTools 优化路线图（精简版）

## 设计原则

1. **简单优先** - 避免过度设计
2. **收益明确** - 只实现有明确性能收益的优化
3. **数据驱动** - 基于 profiling 数据决策

## 已完成

### P0: 核心优化 ✅

| 优化项 | 状态 | 收益 |
|--------|------|------|
| 内存池 | ✅ 完成 | 减少分配开销 |
| libdeflate | ✅ 完成 | 解压速度 2-3x |

## 暂不实施

以下优化经评估后暂不实施：

| 优化项 | 原因 |
|--------|------|
| SIMD | 维护成本高，瓶颈不在计算 |
| 并行解析 | TBB 已足够，复杂度高 |
| 插件系统 | ABI 兼容负担，需求不明确 |
| Huge Pages | 部署复杂，收益不明显 |

## 后续考虑

在有明确 profiling 数据支持时可考虑：

1. SIMD 质量检查（如果成为瓶颈）
2. io_uring（如果 IO 成为瓶颈）

## 验证方法

```bash
# 性能测试
time fqtools filter -i large.fq.gz -o out.fq.gz -t 8

# 热点分析
perf record -g fqtools filter -i large.fq.gz -o out.fq.gz
perf report
```
