#include "tpool/work/manager.hpp"

#include <iostream> // TODO: remove

namespace tpool {

namespace work {

Manager::Manager() : state_(Manager::State::ACCEPTING), work_queue_(), stop_counter_(0, 0, static_cast<int>(1e6)), workers_(), mtx_(), cond_() {}

void Manager::hire(unsigned int count) {
    std::unique_lock<std::mutex> lock(mtx_);

    if (state_ != Manager::State::ACCEPTING)
        throw "Not accepting new workers";

    for (auto& worker : workers_) {
        if (count == 0)
            break;

        if (worker->state() == Worker::State::STOPPED) {
            worker->start();
            --count;
        }
    }

    unsigned int id = 0; // TODO: save last id as data member

    while (count--) {
        std::unique_ptr<Worker> worker = std::make_unique<Worker>(id++, work_queue_, stop_counter_);
        worker->start();
        workers_.push_back(std::move(worker));
    }
}


void Manager::fire(unsigned int count) {
    for (int i = 0; i < count; i++)
        ++stop_counter_; // TODO: use += if possible

    std::unique_lock<std::mutex> lock(mtx_);

    if (state_ != Manager::State::ACCEPTING)
        throw "All workers are stopping";

    for (auto& worker : workers_) {
        if (worker->state() == worker::Worker::State::WAITING && worker->notify()) {
            --count;
        }

        if (count == 0)
            break;
    }
}


void Manager::stopAll() {
    std::unique_lock<std::mutex> lock(mtx_);

    state_ = Manager::State::REFUSING;

    for (auto& worker : workers_)
        worker->stop();
}


void Manager::awaitAllStop() {
    std::unique_lock<std::mutex> lock(mtx_);

    if (state_ != Manager::State::REFUSING)
        throw "Request stop before awaiting";

    lock.unlock();
    // the state prevents anyone to modify workers_, therefore it's safe to access it
    for (auto& worker : workers_) {
        worker->awaitStop();
    }
}


void Manager::delegate(Work work) {
    work_queue_.offer(std::move(work));
    // std::cout << work_queue_.size() << std::endl;

    std::unique_lock<std::mutex> lock(mtx_);

    for (auto& worker : workers_)
        if (worker->notify())
            break;
}


util::SafeQueue<Work>& Manager::workQueue() {
    return work_queue_;
}

}

}