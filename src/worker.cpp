#include "tpool/work/worker.hpp"

namespace tpool {

namespace work {

Worker::Worker(RequestQueue& request_queue)
    : state_(State::STOPPED), thr_(), request_queue_(request_queue), 
      force_stop_(false), mtx_(), cond_() {}


Worker::~Worker() {
    awaitStop();
}


Worker::State Worker::state() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return state_;
}


void Worker::start() {
    std::unique_lock<std::mutex> lock(mtx_);

    if (state_ != Worker::State::STOPPED)
        throw "worker already started";

    thr_ = std::thread(&Worker::loop, this);
}


void Worker::awaitStop() {
    if (thr_.joinable())
        thr_.join();
}


void Worker::loop() {
    std::unique_lock<std::mutex> lock(mtx_);

    while (true) {
        state_ = Worker::State::WAITING;

        lock.unlock();
        Request request = request_queue_.waitRequest();
        lock.lock();

        if (request.shouldStop())
            break;

        state_ = Worker::State::WORKING;
        lock.unlock();
        request.getWork()();
        lock.lock();
    }

    state_ = Worker::State::STOPPED;
}

} // namespace work

} // namespace tpool