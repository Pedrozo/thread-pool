#ifndef TPOOL_WORK_SAFE_QUEUE_HPP_
#define TPOOL_WORK_SAFE_QUEUE_HPP_

#include <queue>
#include <optional>
#include <mutex>

namespace tpool {

namespace work {

template<typename T>
class SafeQueue {
public:

    SafeQueue() : queue_(), mtx_() {}

    std::size_t size() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return queue_.size();
    }

    void offer(T elem) {
        std::unique_lock<std::mutex> lock(mtx_);
        queue_.push(std::move(elem));
    }

    std::optional<T> peek() {
        std::unique_lock<std::mutex> lock(mtx_);
        return next();
    }

    std::optional<T> poll() {
        std::unique_lock<std::mutex> lock(mtx_);
        auto elem = next();

        if (elem)
            queue_.pop();
        
        return elem;
    }

private:
    std::optional<T> next() {
        if (queue_.empty())
            return std::nullopt;
        
        T elem = std::move(queue_.front());

        return std::optional<T>(std::move(elem));
    }

    std::queue<T> queue_;
    mutable std::mutex mtx_;
};

}

}

#endif // TPOOL_WORK_SAFE_QUEUE_HPP_