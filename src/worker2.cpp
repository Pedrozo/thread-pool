#include "tpool/work2/worker.hpp"

namespace tpool {

namespace work2 {

Worker::Worker(SafeQueue<std::unique_ptr<work::Work>>& shared_queue)
    : state_(Worker::State::STOPPED), thr_(), next_work_(nullptr),
      stop_(false), shared_queue_(shared_queue), mtx_(), cond_() {}


Worker::~Worker() {
    std::unique_lock<std::mutex> lock(mtx_);

    if (state_ != Worker::State::STOPPED) {
        stop_ = true;
        lock.unlock();
        cond_.notify_one();
    }

    awaitStop();
}


Worker::State Worker::state() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return state_;
}


void Worker::doWork(std::unique_ptr<work::Work> work) {
    std::unique_lock<std::mutex> lock(mtx_);

    if (next_work_)
        throw "worker already has work to do";

    next_work_ = std::move(work);
}


void Worker::start() {
    std::unique_lock<std::mutex> lock(mtx_);

    if (state_ != Worker::State::STOPPED)
        throw "worker already started";

    stop_ = false;
    thr_ = std::thread(&Worker::loop, this);
    state_ = Worker::State::INITIALIZING;
}


void Worker::stop() {
    std::unique_lock<std::mutex> lock(mtx_);

    if (state_ == Worker::State::STOPPED)
        throw "worker is already stopped";

    stop_ = true;
    lock.unlock();
    cond_.notify_one();
}


void Worker::awaitStop() {
    if (thr_.joinable())
        thr_.join();
}


bool Worker::notify() {
    std::unique_lock<std::mutex> lock(mtx_);
    cond_.notify_one();
    return state_ == Worker::State::WAITING;
}


void Worker::loop() {
    std::unique_lock<std::mutex> lock(mtx_);

    while (!stop_) {
        state_ = Worker::State::WAITING;
        std::unique_ptr<work::Work> work = nullptr;

        cond_.wait(lock, [&] {
            if (next_work_ != nullptr) {
                work = std::move(next_work_);
                return true;
            }

            if (stop_)
                return true;

            auto opt_work = shared_queue_.popIfExists();
            if (opt_work) {
                work = std::move(*opt_work);
                return true;
            }

            return false;
        });

        if (work) {
            state_ = Worker::State::WORKING;
            lock.unlock();
            (*work)();
            lock.lock();
        }
    }

    state_ = Worker::State::STOPPED;
}

} // namespace work

} // namespace tpool