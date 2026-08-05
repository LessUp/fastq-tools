# Memcheck 内存正确性快照

| 字段 | 值 |
|------|----|
| 日期 | 2026-07-13 |
| 工具 | Valgrind 3.22.0 / Memcheck |
| 构建类型 | RelWithDebInfo（-O2 + -g） |
| 数据集 | sample_1k（1K reads × 100 bp） |
| 基线 | [environment.md](../../../environment.md) |

## 调用方式

```bash
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes \
  build/clang-relwithdebinfo/FastQTools stat -i /tmp/sample_1k.fastq -o /dev/null
```

## stat 命令结果

| 指标 | 值 |
|------|----|
| ERROR SUMMARY | **0 errors** from 0 contexts |
| definitely lost | 0 bytes in 0 blocks |
| indirectly lost | 0 bytes in 0 blocks |
| possibly lost | 0 bytes in 0 blocks |
| still reachable | 1,825 bytes in 5 blocks |
| invalid read/write | 0 |
| uninitialised value use | 0 |
| conditional jump on uninit | 0 |

### still reachable 明细（均为动态链接器正常行为）

| 大小 | 来源 |
|------|------|
| 67 bytes × 2 | dlopen 内部加载器状态 |
| 168 bytes | `_dl_new_object`（动态链接对象） |
| 216 bytes | `_dl_map_object_from_fd`（ELF 加载） |
| 1,307 bytes | `_dl_new_object`（主链接映射） |

## filter 命令结果

| 指标 | 值 |
|------|----|
| ERROR SUMMARY | **0 errors** from 0 contexts |
| definitely lost | 0 bytes in 0 blocks |
| indirectly lost | 0 bytes in 0 blocks |
| possibly lost | 0 bytes in 0 blocks |
| still reachable | 1,825 bytes in 5 blocks |
| invalid read/write | 0 |
| uninitialised value use | 0 |
| conditional jump on uninit | 0 |

### still reachable 明细

与 stat 完全一致——同一组动态链接器内部状态，与命令无关。

## 结论

1. **零内存错误**：stat 和 filter 命令均无内存泄露、越界、未初始化使用。
2. **零 definitely lost**：所有分配在退出前释放。
3. **still reachable 1,825 bytes**：全部来自 glibc 动态链接器（`ld-linux`）的 `dlopen` 内部状态，是 libc 正常行为，非项目代码泄露。Massif 快照证实 `dlopen` 由 oneTBB `dynamic_load` 发起（加载 `libtbbbind`）。
4. **`--track-origins=yes` 无发现**：未初始化值追踪未发现任何问题。

## 原始数据

- [stat-memcheck.txt](./stat-memcheck.txt) — stat 命令完整 Memcheck 输出
- [filter-memcheck.txt](./filter-memcheck.txt) — filter 命令完整 Memcheck 输出
