#include "tpool/work2/manager.hpp"

namespace tpool {

namespace work2 {

Manager::Manager() : work_queue_(), workers_(), mtx_(), cond_() {}

Manager::~Manager() {
    // TODO
}

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
        std::unique_ptr<Worker> worker = std::make_unique<Worker>(work_queue_);
        worker->start();
        workers_.push_back(std::move(worker));
    }
}


void Manager::fire(unsigned int count) {
    // TODO
}


void Manager::delegate(std::unique_ptr<work::Work> work) {
    std::unique_lock<std::mutex> lock(mtx_);

    work_queue_.push(std::move(work));

    for (auto& worker : workers_)
        if (worker->notify())
            break;
}

}

}