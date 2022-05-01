#include "tpool/worker/worker.hpp"

namespace tpool {

namespace worker {

Worker::Worker(unsigned int id, util::SafeQueue<work::Work>& shared_queue, util::BoundedCounter<int>& stop_counter)
    : id_(id), state_(Worker::State::STOPPED), thr_(), stop_(false), next_work_(),
      shared_queue_(shared_queue), stop_counter_(stop_counter), mtx_(), cond_() {}


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


void Worker::doWork(work::Work work) {
    std::unique_lock<std::mutex> lock(mtx_);

    if (next_work_)
        throw "worker already has work to do";

    next_work_ = std::optional<work::Work>(std::move(work));

    if (state_ == Worker::State::WAITING) {
        state_ = Worker::State::NOTIFIED;
        lock.unlock();
        cond_.notify_one();
    }
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

    if (state_ == Worker::State::WAITING) {
        state_ = Worker::State::NOTIFIED;
        lock.unlock();
        cond_.notify_one();
    }
}


void Worker::awaitStop() {
    if (thr_.joinable())
        thr_.join();
}


bool Worker::notify() {
    std::unique_lock<std::mutex> lock(mtx_);
    
    if (state_ == Worker::State::WAITING) {
        state_ = Worker::State::NOTIFIED;
        lock.unlock();
        cond_.notify_one();
        return true;
    }

    return false;
}


void Worker::loop() {
    std::unique_lock<std::mutex> lock(mtx_);

    while (next_work_ || !stop_) {
        std::optional<work::Work> work; // TODO: test performance when it is outside the loop

        cond_.wait(lock, [&] {
            state_ = Worker::State::WAITING;

            if (next_work_) {
                work = std::move(next_work_);
                next_work_ = std::nullopt;
                return true;
            }

            if (stop_ = (stop_ || --stop_counter_)) {
                return true;
            }

            work = shared_queue_.poll();

            return bool(work);
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

} // namespace worker

} // namespace tpool