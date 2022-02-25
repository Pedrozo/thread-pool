#ifndef TPOOL_WORK_QUEUE_HPP_
#define TPOOL_WORK_QUEUE_HPP_

#include <queue>
#include <memory>
#include <mutex>
#include "work.hpp"

namespace tpool {

class WorkQueue {
public:

    /*
    void push(std::unique_ptr<Work> work) {
        std::unique_lock<std::mutex> lock(mtx_);
        works_.push(std::move(work));
        lock.unlock();
        cond_.notify_one();
    }

    std::unique_ptr<Work> wait() {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [&] {
            return !works_.empty();
        });
        std::unique_ptr<Work> work = std::move(works_.front());
        works_.pop();
        return work;
    }
    */

private:
    std::queue<std::unique_ptr<Work>> works_;

    /*
    std::queue<std::unique_ptr<Work>> works_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
    */
};

} // namespace tpool

#endif // TPOOL_WORK_QUEUE_HPP_