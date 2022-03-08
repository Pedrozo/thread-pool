#include "tpool/work/manager.hpp"
#include <iostream>

namespace tpool {

namespace work {

Manager::Manager() : work_queue_(), stop_counter_(0, 0, static_cast<int>(1e6)), workers_(), mtx_(), cond_() {}

void Manager::hire(unsigned int count) {
    std::unique_lock<std::mutex> lock(mtx_);

    for (auto& worker : workers_) {
        if (count == 0)
            break;

        if (worker->state() == Worker::State::STOPPED) {
            worker->start();
            --count;
        }
    }

    while (count--) {
        std::unique_ptr<Worker> worker = std::make_unique<Worker>(work_queue_, stop_counter_);
        worker->start();
        workers_.push_back(std::move(worker));
    }
}


void Manager::fire(unsigned int count) {
    for (int i = 0; i < count; i++)
        ++stop_counter_; // TODO: use += if possible

    std::unique_lock<std::mutex> lock(mtx_);

    for (auto& worker : workers_) {
        if (worker->state() == Worker::State::WAITING && worker->notify()) {
            --count;
        }

        if (count == 0)
            break;
    }
}


void Manager::delegate(Work work) {
    work_queue_.offer(std::move(work));

    std::unique_lock<std::mutex> lock(mtx_);

    for (auto& worker : workers_)
        if (worker->notify())
            break;
}

}

}