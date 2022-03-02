#include "tpool/work/request_queue.hpp"

namespace tpool {

namespace work {

RequestQueue::RequestQueue() : work_queue_(), stop_count_(0), mtx_(), cond_() {}

std::size_t RequestQueue::stopCount() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return stop_count_;
}


std::size_t RequestQueue::workCount() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return work_queue_.size();
}


void RequestQueue::addStop(std::size_t count) {
    std::unique_lock<std::mutex> lock(mtx_);
    stop_count_ += count;

    lock.unlock();

    if (count > 1)
        cond_.notify_all();
    else
        cond_.notify_one();
}


void RequestQueue::addWork(std::unique_ptr<Work> work) {
    std::unique_lock<std::mutex> lock(mtx_);
    work_queue_.push(std::move(work));
    lock.unlock();
    cond_.notify_one();
}


Request RequestQueue::waitRequest() {
    std::unique_lock<std::mutex> lock(mtx_);

    cond_.wait(lock, [&] {
        return stop_count_ > 0 || work_queue_.size() > 0;
    });

    if (work_queue_.size() > 0) {
        Request inst(std::move(work_queue_.front()));
        work_queue_.pop();
        return inst;
    } else {
        --stop_count_;
        return Request(nullptr);
    }
}


} // namespace work

} // namespace tpool
