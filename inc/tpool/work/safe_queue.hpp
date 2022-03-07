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

    void push(T elem) {
        std::unique_lock<std::mutex> lock(mtx_);
        queue_.push(std::move(elem));
    }

    std::optional<T> popIfExists() {
        std::unique_lock<std::mutex> lock(mtx_);

        if (queue_.empty())
            return std::nullopt;
        
        T elem = std::move(queue_.front());
        queue_.pop();

        return std::optional<T>(std::move(elem));

        // return { elem }; // this will copy?
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mtx_;
};

}

}

#endif // TPOOL_WORK2_SAFE_QUEUE_HPP_