# Implementation Tasks: Memory Pool

## 已完成任务

### Task 1: ObjectPool<T> 模板类 ✅

- [x] 创建 `include/fqtools/memory/object_pool.h`
- [x] 实现 acquire() 方法（含自定义删除器）
- [x] 实现 reserve() 预分配方法
- [x] 实现 shrink() 收缩方法
- [x] 实现监控方法

**文件**: `include/fqtools/memory/object_pool.h`

### Task 2: FastqBatchPool ✅

- [x] 创建 `include/fqtools/io/fastq_batch_pool.h`
- [x] 实现 resetFastqBatch() 函数
- [x] 实现 createFastqBatchPool() 工厂函数

**文件**: `include/fqtools/io/fastq_batch_pool.h`

### Task 3: Pipeline 集成 ✅

- [x] 修改 `src/processing/processing_pipeline.cpp`
- [x] 在 processWithTBB() 中使用对象池
- [x] 配置池大小基于 maxTokens

**文件**: `src/processing/processing_pipeline.cpp`

### Task 4: 单元测试 ✅

- [x] 创建 `tests/unit/memory/test_object_pool.cpp`
- [x] 测试基础功能
- [x] 测试并发安全
- [x] 更新 CMakeLists.txt

**文件**: `tests/unit/memory/test_object_pool.cpp`

## 待完成任务

### Task 5: 构建验证

- [ ] 在完整构建环境中编译
- [ ] 运行单元测试
- [ ] 验证无编译警告

### Task 6: 性能验证

- [ ] 对比有/无内存池的吞吐量
- [ ] 验证内存分配次数减少
