// Runtime-internal object pool. It is intentionally not part of the installed API.
#pragma once

#include <atomic>
#include <functional>
#include <memory>

#include <tbb/concurrent_queue.h>

namespace fq::memory {

template <typename T>
class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>> {
public:
    using ResetFunc = std::function<void(T&)>;
    using CreateFunc = std::function<std::unique_ptr<T>()>;

    explicit ObjectPool(size_t initialSize = 0,
                        size_t maxSize = 0,
                        ResetFunc resetFunc = nullptr,
                        CreateFunc createFunc = nullptr)
        : resetFunc_(std::move(resetFunc)), createFunc_(std::move(createFunc)) {
        if (maxSize > 0) {
            queue_.set_capacity(maxSize);
        }
        if (initialSize > 0) {
            reserve(initialSize);
        }
    }

    ObjectPool(const ObjectPool&) = delete;
    auto operator=(const ObjectPool&) -> ObjectPool& = delete;
    ObjectPool(ObjectPool&&) = delete;
    auto operator=(ObjectPool&&) -> ObjectPool& = delete;
    ~ObjectPool() = default;

    [[nodiscard]] auto acquire() -> std::shared_ptr<T> {
        std::unique_ptr<T> object;
        if (!queue_.try_pop(object)) {
            object = createObject();
            totalCreated_.fetch_add(1, std::memory_order_relaxed);
        }
        if (resetFunc_) {
            resetFunc_(*object);
        }
        activeCount_.fetch_add(1, std::memory_order_relaxed);

        std::weak_ptr<ObjectPool<T>> weakThis = this->shared_from_this();
        return std::shared_ptr<T>(object.release(), [weakThis](T* pointer) {
            if (auto pool = weakThis.lock()) {
                pool->releaseImpl(std::unique_ptr<T>(pointer));
            } else {
                delete pointer;
            }
        });
    }

    void reserve(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            auto object = createObject();
            if (!queue_.try_push(std::move(object))) {
                break;
            }
            totalCreated_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void shrink() noexcept {
        std::unique_ptr<T> object;
        while (queue_.try_pop(object)) {
            // 对象析构释放
        }
    }

    [[nodiscard]] auto poolSize() const -> size_t {
        return queue_.size();
    }

    [[nodiscard]] auto activeCount() const -> size_t {
        return activeCount_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] auto totalCreated() const -> size_t {
        return totalCreated_.load(std::memory_order_relaxed);
    }

private:
    [[nodiscard]] auto createObject() const -> std::unique_ptr<T> {
        return createFunc_ ? createFunc_() : std::make_unique<T>();
    }

    void releaseImpl(std::unique_ptr<T> object) {
        activeCount_.fetch_sub(1, std::memory_order_relaxed);
        // try_push 不阻塞；队列满时返回 false，对象由 unique_ptr 析构释放
        queue_.try_push(std::move(object));
    }

    tbb::concurrent_bounded_queue<std::unique_ptr<T>> queue_;
    ResetFunc resetFunc_;
    CreateFunc createFunc_;
    std::atomic<size_t> activeCount_{0};
    std::atomic<size_t> totalCreated_{0};
};

}  // namespace fq::memory
