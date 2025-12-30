/**
 * @file object_pool.h
 * @brief 通用对象池模板类
 * @details 提供线程安全的对象复用机制，减少高频内存分配开销
 *
 * @author FastQTools Team
 * @date 2024
 * @version 1.0
 *
 * @copyright Copyright (c) 2024 FastQTools
 * @license MIT License
 */

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace fq::memory {

/**
 * @brief 通用对象池模板类
 * @tparam T 池化对象类型，必须支持默认构造
 *
 * @details 对象池通过复用对象减少内存分配开销。使用 shared_ptr 的自定义删除器
 *          实现自动归还机制，无需显式调用 release。
 *
 * @note 线程安全：所有公共方法都是线程安全的
 *
 * @example
 * @code
 * ObjectPool<MyObject> pool(4, 16, [](MyObject& obj) { obj.reset(); });
 * auto obj = pool.acquire();  // 获取对象
 * // 使用 obj...
 * // obj 析构时自动归还到池中
 * @endcode
 */
template <typename T>
class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>> {
public:
    /// 对象重置函数类型
    using ResetFunc = std::function<void(T&)>;

    /**
     * @brief 构造对象池
     * @param initialSize 初始预分配对象数量
     * @param maxSize 池的最大容量，0 表示无限制
     * @param resetFunc 对象重置回调函数，获取对象时调用
     */
    explicit ObjectPool(size_t initialSize = 0,
                        size_t maxSize = 0,
                        ResetFunc resetFunc = nullptr)
        : resetFunc_(std::move(resetFunc)), maxSize_(maxSize) {
        if (initialSize > 0) {
            reserve(initialSize);
        }
    }

    /// 禁止拷贝
    ObjectPool(const ObjectPool&) = delete;
    auto operator=(const ObjectPool&) -> ObjectPool& = delete;

    /// 禁止移动（因为 enable_shared_from_this）
    ObjectPool(ObjectPool&&) = delete;
    auto operator=(ObjectPool&&) -> ObjectPool& = delete;

    ~ObjectPool() = default;

    /**
     * @brief 从池中获取对象
     * @return 指向对象的 shared_ptr，析构时自动归还
     * @throws std::bad_alloc 如果内存分配失败
     *
     * @details 如果池中有可用对象则复用，否则创建新对象。
     *          返回的 shared_ptr 使用自定义删除器，析构时自动归还对象到池中。
     */
    [[nodiscard]] auto acquire() -> std::shared_ptr<T> {
        std::unique_ptr<T> obj;

        {
            std::lock_guard lock(mutex_);
            if (!pool_.empty()) {
                obj = std::move(pool_.back());
                pool_.pop_back();
            }
        }

        if (!obj) {
            obj = std::make_unique<T>();
            totalCreated_.fetch_add(1, std::memory_order_relaxed);
        }

        if (resetFunc_) {
            resetFunc_(*obj);
        }

        activeCount_.fetch_add(1, std::memory_order_relaxed);

        // 使用 weak_ptr 防止循环引用，池销毁后对象直接删除
        std::weak_ptr<ObjectPool<T>> weakThis = this->shared_from_this();

        return std::shared_ptr<T>(obj.release(), [weakThis](T* ptr) {
            if (auto pool = weakThis.lock()) {
                pool->releaseImpl(std::unique_ptr<T>(ptr));
            } else {
                delete ptr;
            }
        });
    }

    /**
     * @brief 预分配指定数量的对象
     * @param count 要预分配的对象数量
     * @throws std::bad_alloc 如果内存分配失败
     */
    void reserve(size_t count) {
        std::lock_guard lock(mutex_);
        pool_.reserve(pool_.size() + count);

        for (size_t i = 0; i < count; ++i) {
            if (maxSize_ > 0 && pool_.size() >= maxSize_) {
                break;
            }
            pool_.push_back(std::make_unique<T>());
            totalCreated_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    /**
     * @brief 收缩池，释放所有未使用的对象
     */
    void shrink() noexcept {
        std::lock_guard lock(mutex_);
        pool_.clear();
        pool_.shrink_to_fit();
    }

    /**
     * @brief 获取当前池中可用对象数量
     * @return 可用对象数量
     */
    [[nodiscard]] auto poolSize() const -> size_t {
        std::lock_guard lock(mutex_);
        return pool_.size();
    }

    /**
     * @brief 获取当前被借出的对象数量
     * @return 活跃对象数量
     */
    [[nodiscard]] auto activeCount() const -> size_t {
        return activeCount_.load(std::memory_order_relaxed);
    }

    /**
     * @brief 获取累计创建的对象总数
     * @return 创建的对象总数
     */
    [[nodiscard]] auto totalCreated() const -> size_t {
        return totalCreated_.load(std::memory_order_relaxed);
    }

private:
    /**
     * @brief 归还对象到池中（内部实现）
     * @param obj 要归还的对象
     */
    void releaseImpl(std::unique_ptr<T> obj) {
        activeCount_.fetch_sub(1, std::memory_order_relaxed);

        std::lock_guard lock(mutex_);
        if (maxSize_ == 0 || pool_.size() < maxSize_) {
            pool_.push_back(std::move(obj));
        }
        // 超过 maxSize 时，obj 会被自动销毁
    }

    std::vector<std::unique_ptr<T>> pool_;
    mutable std::mutex mutex_;
    ResetFunc resetFunc_;
    size_t maxSize_;
    std::atomic<size_t> activeCount_{0};
    std::atomic<size_t> totalCreated_{0};
};

}  // namespace fq::memory
