/**
 * @file test_object_pool.cpp
 * @brief ObjectPool 单元测试
 */

#include "fqtools/memory/object_pool.h"
#include "fqtools/io/fastq_batch_pool.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <atomic>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

namespace fq::memory::test {

// 测试用的简单对象
class TestObject {
public:
    TestObject() : value_(0), resetCount_(0) {}
    
    void setValue(int v) { value_ = v; }
    [[nodiscard]] auto value() const -> int { return value_; }
    
    void reset() { 
        value_ = 0; 
        resetCount_++;
    }
    [[nodiscard]] auto resetCount() const -> int { return resetCount_; }

private:
    int value_;
    int resetCount_;
};

class ObjectPoolTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// REQ1-AC2: 空池时创建新对象
TEST_F(ObjectPoolTest, AcquireCreatesNewObjectWhenPoolEmpty) {
    auto pool = std::make_shared<ObjectPool<TestObject>>();
    
    EXPECT_EQ(pool->poolSize(), 0);
    EXPECT_EQ(pool->activeCount(), 0);
    EXPECT_EQ(pool->totalCreated(), 0);
    
    auto obj = pool->acquire();
    
    EXPECT_NE(obj, nullptr);
    EXPECT_EQ(pool->activeCount(), 1);
    EXPECT_EQ(pool->totalCreated(), 1);
}

// REQ1-AC1: 复用池中对象
TEST_F(ObjectPoolTest, AcquireReusesPooledObject) {
    auto pool = std::make_shared<ObjectPool<TestObject>>();
    
    TestObject* rawPtr = nullptr;
    {
        auto obj = pool->acquire();
        rawPtr = obj.get();
        obj->setValue(42);
    }
    // obj 析构，应该归还到池中
    
    EXPECT_EQ(pool->poolSize(), 1);
    EXPECT_EQ(pool->activeCount(), 0);
    
    auto obj2 = pool->acquire();
    EXPECT_EQ(obj2.get(), rawPtr);  // 应该是同一个对象
    EXPECT_EQ(pool->totalCreated(), 1);  // 没有创建新对象
}

// REQ1-AC3: shared_ptr 销毁时自动归还
TEST_F(ObjectPoolTest, AutoReleaseOnSharedPtrDestruction) {
    auto pool = std::make_shared<ObjectPool<TestObject>>();
    
    {
        auto obj1 = pool->acquire();
        auto obj2 = pool->acquire();
        EXPECT_EQ(pool->activeCount(), 2);
        EXPECT_EQ(pool->poolSize(), 0);
    }
    
    EXPECT_EQ(pool->activeCount(), 0);
    EXPECT_EQ(pool->poolSize(), 2);
}

// REQ1-AC4: 预分配指定数量对象
TEST_F(ObjectPoolTest, ReservePreallocatesObjects) {
    auto pool = std::make_shared<ObjectPool<TestObject>>();
    
    pool->reserve(5);
    
    EXPECT_EQ(pool->poolSize(), 5);
    EXPECT_EQ(pool->totalCreated(), 5);
    EXPECT_EQ(pool->activeCount(), 0);
}

// REQ1-AC5: 监控方法正确性
TEST_F(ObjectPoolTest, MonitoringMethodsCorrectness) {
    auto pool = std::make_shared<ObjectPool<TestObject>>(3, 10);
    
    EXPECT_EQ(pool->poolSize(), 3);
    EXPECT_EQ(pool->totalCreated(), 3);
    EXPECT_EQ(pool->activeCount(), 0);
    
    auto obj1 = pool->acquire();
    EXPECT_EQ(pool->poolSize(), 2);
    EXPECT_EQ(pool->activeCount(), 1);
    
    auto obj2 = pool->acquire();
    auto obj3 = pool->acquire();
    EXPECT_EQ(pool->poolSize(), 0);
    EXPECT_EQ(pool->activeCount(), 3);
    
    auto obj4 = pool->acquire();  // 创建新对象
    EXPECT_EQ(pool->totalCreated(), 4);
    EXPECT_EQ(pool->activeCount(), 4);
}

// REQ3-AC1,2: 获取的对象已重置
TEST_F(ObjectPoolTest, ObjectResetOnAcquire) {
    auto resetFunc = [](TestObject& obj) { obj.reset(); };
    auto pool = std::make_shared<ObjectPool<TestObject>>(0, 0, resetFunc);
    
    TestObject* rawPtr = nullptr;
    {
        auto obj = pool->acquire();
        rawPtr = obj.get();
        obj->setValue(42);
        EXPECT_EQ(obj->value(), 42);
        EXPECT_EQ(obj->resetCount(), 1);  // acquire 时调用了 reset
    }
    
    auto obj2 = pool->acquire();
    EXPECT_EQ(obj2.get(), rawPtr);
    EXPECT_EQ(obj2->value(), 0);  // 值已重置
    EXPECT_EQ(obj2->resetCount(), 2);  // 再次调用了 reset
}

// REQ4-AC1,2: 超限时丢弃对象
TEST_F(ObjectPoolTest, MaxSizeLimitDiscardsExcessObjects) {
    auto pool = std::make_shared<ObjectPool<TestObject>>(0, 2);
    
    std::vector<std::shared_ptr<TestObject>> objects;
    for (int i = 0; i < 5; ++i) {
        objects.push_back(pool->acquire());
    }
    EXPECT_EQ(pool->activeCount(), 5);
    EXPECT_EQ(pool->totalCreated(), 5);
    
    // 释放所有对象
    objects.clear();
    
    // 池大小应该被限制在 maxSize
    EXPECT_EQ(pool->poolSize(), 2);
    EXPECT_EQ(pool->activeCount(), 0);
}

// REQ4-AC3: shrink 释放池中对象
TEST_F(ObjectPoolTest, ShrinkReleasesPooledObjects) {
    auto pool = std::make_shared<ObjectPool<TestObject>>(5, 10);
    
    EXPECT_EQ(pool->poolSize(), 5);
    
    pool->shrink();
    
    EXPECT_EQ(pool->poolSize(), 0);
}

// REQ2-AC1: 并发获取返回不同对象
TEST_F(ObjectPoolTest, ConcurrentAcquireReturnsDistinctObjects) {
    auto pool = std::make_shared<ObjectPool<TestObject>>(0, 100);
    
    constexpr int kThreadCount = 8;
    constexpr int kAcquiresPerThread = 10;
    
    std::vector<std::thread> threads;
    std::mutex mutex;
    std::set<TestObject*> allPointers;
    // 关键修复：全局持有所有对象，防止提前释放导致指针复用
    std::vector<std::shared_ptr<TestObject>> allObjects;
    std::atomic<int> successCount{0};
    
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&]() {
            std::vector<std::shared_ptr<TestObject>> localObjects;
            for (int i = 0; i < kAcquiresPerThread; ++i) {
                auto obj = pool->acquire();
                if (obj) {
                    localObjects.push_back(obj);
                    successCount++;
                }
            }
            
            // 加锁插入指针并转移对象所有权到全局容器
            std::lock_guard lock(mutex);
            for (const auto& obj : localObjects) {
                auto [it, inserted] = allPointers.insert(obj.get());
                EXPECT_TRUE(inserted) << "Duplicate pointer detected!";
            }
            // 将对象移动到全局容器，确保对象生命周期持续到测试结束
            allObjects.insert(allObjects.end(), 
                              std::make_move_iterator(localObjects.begin()),
                              std::make_move_iterator(localObjects.end()));
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(successCount.load(), kThreadCount * kAcquiresPerThread);
    EXPECT_EQ(allPointers.size(), static_cast<size_t>(kThreadCount * kAcquiresPerThread));
    // allObjects 在此处析构，所有对象才会被释放
}

// REQ2-AC2: 并发释放正确归还
TEST_F(ObjectPoolTest, ConcurrentReleaseCorrectlyReturnsObjects) {
    auto pool = std::make_shared<ObjectPool<TestObject>>(0, 100);
    
    constexpr int kObjectCount = 50;
    
    // 先获取一批对象
    std::vector<std::shared_ptr<TestObject>> objects;
    for (int i = 0; i < kObjectCount; ++i) {
        objects.push_back(pool->acquire());
    }
    
    EXPECT_EQ(pool->activeCount(), kObjectCount);
    
    // 并发释放
    std::vector<std::thread> threads;
    for (int i = 0; i < kObjectCount; ++i) {
        threads.emplace_back([obj = std::move(objects[i])]() mutable {
            obj.reset();  // 释放对象
        });
    }
    objects.clear();
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(pool->activeCount(), 0);
    EXPECT_EQ(pool->poolSize(), kObjectCount);
}

// 测试池销毁后对象安全删除
TEST_F(ObjectPoolTest, SafeDeleteWhenPoolDestroyed) {
    std::shared_ptr<TestObject> obj;
    
    {
        auto pool = std::make_shared<ObjectPool<TestObject>>();
        obj = pool->acquire();
        EXPECT_EQ(pool->activeCount(), 1);
    }
    // pool 已销毁
    
    // obj 应该仍然有效，析构时直接删除而不是归还
    EXPECT_NE(obj, nullptr);
    obj.reset();  // 不应崩溃
}

// FastqBatchPool 集成测试
TEST_F(ObjectPoolTest, FastqBatchPoolIntegration) {
    auto pool = fq::io::createFastqBatchPool(2, 4);
    
    EXPECT_EQ(pool->poolSize(), 2);
    
    auto batch = pool->acquire();
    EXPECT_NE(batch, nullptr);
    EXPECT_TRUE(batch->empty());
    
    // 模拟使用
    batch->buffer().resize(100);
    
    // 释放后应该被重置
    batch.reset();
    
    auto batch2 = pool->acquire();
    EXPECT_TRUE(batch2->empty());
    EXPECT_TRUE(batch2->buffer().empty());
}

}  // namespace fq::memory::test
