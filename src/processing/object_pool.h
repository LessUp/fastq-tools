// Runtime-internal object pool. It is intentionally not part of the installed API.
#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

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
        : resetFunc_(std::move(resetFunc)), createFunc_(std::move(createFunc)), maxSize_(maxSize) {
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
        {
            std::lock_guard lock(mutex_);
            if (!pool_.empty()) {
                object = std::move(pool_.back());
                pool_.pop_back();
            }
        }

        if (!object) {
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
        std::lock_guard lock(mutex_);
        pool_.reserve(pool_.size() + count);
        for (size_t i = 0; i < count; ++i) {
            if (maxSize_ > 0 && pool_.size() >= maxSize_) {
                break;
            }
            pool_.push_back(createObject());
            totalCreated_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void shrink() noexcept {
        std::lock_guard lock(mutex_);
        pool_.clear();
        pool_.shrink_to_fit();
    }

    [[nodiscard]] auto poolSize() const -> size_t {
        std::lock_guard lock(mutex_);
        return pool_.size();
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
        std::lock_guard lock(mutex_);
        if (maxSize_ == 0 || pool_.size() < maxSize_) {
            pool_.push_back(std::move(object));
        }
    }

    std::vector<std::unique_ptr<T>> pool_;
    mutable std::mutex mutex_;
    ResetFunc resetFunc_;
    CreateFunc createFunc_;
    size_t maxSize_;
    std::atomic<size_t> activeCount_{0};
    std::atomic<size_t> totalCreated_{0};
};

}  // namespace fq::memory
