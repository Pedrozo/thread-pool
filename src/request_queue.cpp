#include "tpool/work/request_queue.hpp"

namespace tpool {

namespace work {

RequestQueue::RequestQueue() : work_queue_(), stop_count_(0), always_stop_(false), mtx_(), cond_() {}


std::size_t RequestQueue::stopCount() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return stop_count_;
}


std::size_t RequestQueue::workCount() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return work_queue_.size();
}

std::size_t RequestQueue::size() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return stop_count_ + work_queue_.size();
}

void RequestQueue::resetStopCount() {
    std::unique_lock<std::mutex> lock(mtx_);
    stop_count_ = 0;
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
        return stop_count_ > 0 || work_queue_.size() > 0 || always_stop_;
    });

    if (work_queue_.size() > 0) {
        Request request = makeWorkRequest(std::move(work_queue_.front()));
        work_queue_.pop();
        return request;
    } else if (stop_count_ > 0) {
        --stop_count_;
    }

    return makeStopRequest();
}


void RequestQueue::setToAlwaysStop(bool always_stop) {
    std::unique_lock<std::mutex> lock(mtx_);
    always_stop_ = always_stop;
}


} // namespace work

} // namespace tpool
