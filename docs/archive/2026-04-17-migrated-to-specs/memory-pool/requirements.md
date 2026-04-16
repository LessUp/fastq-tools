# Requirements Document: Memory Pool

## Introduction

本文档定义了 FastQTools 项目中 FastqBatch 对象池的需求规范。对象池旨在减少高频内存分配开销，提升并行处理流水线的性能。

## Glossary

- **Object_Pool**: 预分配对象的缓存池，支持对象的获取和归还
- **FastqBatch**: FASTQ 记录的批量容器，包含缓冲区和记录元数据
- **Pipeline**: TBB 并行处理流水线
- **Acquire**: 从对象池获取对象的操作
- **Release**: 将对象归还到对象池的操作

## Requirements

### Requirement 1: 对象池基础功能

**User Story:** As a pipeline developer, I want to reuse FastqBatch objects, so that I can reduce memory allocation overhead in hot paths.

#### Acceptance Criteria

1. THE Object_Pool SHALL provide an `acquire()` method that returns a shared pointer to a pooled object
2. WHEN the pool is empty, THE Object_Pool SHALL create a new object and return it
3. WHEN a pooled object's shared pointer is destroyed, THE Object_Pool SHALL automatically return the object to the pool
4. THE Object_Pool SHALL support pre-allocation of objects via a `reserve(count)` method
5. THE Object_Pool SHALL provide `poolSize()` and `activeCount()` methods for monitoring

### Requirement 2: 线程安全

**User Story:** As a concurrent pipeline user, I want the object pool to be thread-safe, so that multiple threads can safely acquire and release objects.

#### Acceptance Criteria

1. WHEN multiple threads call `acquire()` simultaneously, THE Object_Pool SHALL return distinct objects to each thread
2. WHEN multiple threads release objects simultaneously, THE Object_Pool SHALL correctly return all objects to the pool
3. THE Object_Pool SHALL use mutex to ensure thread safety

### Requirement 3: 对象重置

**User Story:** As a batch processor, I want acquired objects to be in a clean state, so that I don't need to manually clear previous data.

#### Acceptance Criteria

1. WHEN an object is acquired from the pool, THE Object_Pool SHALL ensure the object is in a cleared/reset state
2. THE Object_Pool SHALL call a configurable reset function before returning objects

### Requirement 4: 内存限制

**User Story:** As a system administrator, I want to limit the pool size, so that memory usage stays within acceptable bounds.

#### Acceptance Criteria

1. THE Object_Pool SHALL support an optional maximum pool size configuration
2. WHEN the pool reaches maximum size, THE Object_Pool SHALL discard returned objects instead of pooling them
3. THE Object_Pool SHALL provide a `shrink()` method to release unused pooled objects

### Requirement 5: Pipeline 集成

**User Story:** As a pipeline implementer, I want to integrate the object pool with TBB parallel_pipeline, so that batch objects are efficiently reused across pipeline stages.

#### Acceptance Criteria

1. THE Processing_Pipeline SHALL use Object_Pool for FastqBatch allocation in the source stage
2. WHEN a batch completes processing, THE Processing_Pipeline SHALL automatically return it to the pool via shared_ptr destructor
