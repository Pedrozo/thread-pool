#ifndef TPOOL_UTIL_SAFE_QUEUE_HPP_
#define TPOOL_UTIL_SAFE_QUEUE_HPP_

#include <queue>
#include <optional>
#include <mutex>

namespace tpool {

namespace util {

/**
 * A thread-safe queue.
 */
template<typename T>
class SafeQueue {
public:

    SafeQueue() : queue_(), mtx_() {}

    SafeQueue(const SafeQueue& other)
        : SafeQueue(other, std::lock_guard<std::mutex>(other.mtx_)) {}

    SafeQueue(SafeQueue&& other) noexcept : queue_(std::move(other.queue_)), mtx_() {}

    SafeQueue& operator=(const SafeQueue& other) {
        std::scoped_lock lock(mtx_, other.mtx_); // C++ 17
        queue_ = other.queue_;
        return *this;
    }

    SafeQueue& operator=(SafeQueue&& other) noexcept {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_ = std::move(other.queue_);
        return *this;
    }

    /**
     * Returns the queue size.
     *
     * @return the queue size
     */
    std::size_t size() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return queue_.size();
    }

    /**
     * Inserts an element at the end of the queue.
     *
     * @param elem the element to be inserted
     */
    void offer(T elem) {
        std::unique_lock<std::mutex> lock(mtx_);
        queue_.push(std::move(elem));
    }

    /**
     * Retrieves and removes the next element of the queue, if the queue is not empty. Otherwise, it returns
     * a empty optional. Such operation makes more sense in a thread safe queue, since it avoids
     * the race condition between verifying emptiness, retrieving and removing the next element.
     */
    std::optional<T> poll() {
        std::unique_lock<std::mutex> lock(mtx_);
        auto elem = next();

        if (elem)
            queue_.pop();

        return elem;
    }

private:
    SafeQueue(const SafeQueue& other, const std::lock_guard<std::mutex>& lock)
        : queue_(other.queue_), mtx_() {}

    std::optional<T> next() {
        if (queue_.empty())
            return std::nullopt;

        T elem = std::move(queue_.front());

        return std::optional<T>(std::move(elem));
    }

    std::queue<T> queue_;
    mutable std::mutex mtx_;
};

} // namespace util

} // namespace tpool

#endif // TPOOL_UTIL_SAFE_QUEUE_HPP_