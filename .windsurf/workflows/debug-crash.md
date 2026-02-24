---
title: "/debug-crash"
description: "调试崩溃/段错误：GDB 附加、core dump 分析、堆栈解读，快速定位崩溃根因。"
auto_execution_mode: 1
---

## 适用场景
- 程序崩溃（SIGSEGV、SIGABRT、assert 失败）
- 测试偶发性 crash
- 需要分析 core dump 文件

## 工作流步骤
1. **启用 core dump（如果未启用）**
   - `ulimit -c unlimited`
   - 确认 core dump 路径：`cat /proc/sys/kernel/core_pattern`

2. **用 Debug 模式构建（保留符号信息）**
   // turbo
   - `./scripts/core/build -c clang -t Debug`

3. **复现崩溃**
   - 直接运行触发崩溃的命令，例如：
     - `./build-clang-debug/FastQTools stat <input_file>`
   - 或运行特定测试：
     - `./scripts/core/test -c clang -t Debug -f "<crash_test_pattern>"`

4. **GDB 分析**
   - **直接调试**：`gdb --args ./build-clang-debug/FastQTools stat <input_file>`
     - `run` → 等待崩溃
     - `bt` → 查看完整回溯
     - `bt full` → 查看带局部变量的回溯
     - `info locals` → 当前帧局部变量
     - `frame N` → 切换到第 N 帧
     - `print <var>` → 查看变量值
   - **分析 core dump**：`gdb ./build-clang-debug/FastQTools core`
     - `bt` → 查看崩溃时的回溯

5. **定位根因**
   - 从堆栈最底层（用户代码帧）开始分析
   - 检查空指针、越界、iterator 失效等常见原因
   - 如果堆栈不清晰，考虑配合 ASan：`/asan-check`

6. **修复与验证**
   - 修复代码
   - 重新构建并运行触发崩溃的测试
   - 添加回归测试覆盖该场景

## 常用 GDB 命令速查
| 命令 | 用途 |
|------|------|
| `bt` / `backtrace` | 完整调用栈 |
| `bt full` | 调用栈 + 局部变量 |
| `frame N` | 切换到第 N 帧 |
| `info locals` | 当前帧局部变量 |
| `print expr` | 求值并打印 |
| `watch expr` | 设置数据断点 |
| `catch throw` | 在异常抛出时断下 |
| `thread apply all bt` | 所有线程的调用栈 |

## 备注
- Debug 构建已包含 `-g` 和 `-O0`，符号信息完整
- 如果怀疑是优化导致的问题，尝试 Release + `-g`：`cmake --build ... -- -DCMAKE_CXX_FLAGS="-g"`
- 多线程崩溃优先用 `/tsan-check` 排查竞态
