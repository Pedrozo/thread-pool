#include "tpool/work/manager.hpp"

namespace tpool {

namespace work {

Manager::Manager()
    : request_queue_(), workers_(), state_(Manager::State::ACCEPTING), mtx_(), cond_() {}


Manager::~Manager() {
    std::unique_lock<std::mutex> lock(mtx_);
	request_queue_.addStop(workers_.size());
}


void Manager::hire(unsigned int count) {
	std::unique_lock<std::mutex> lock(mtx_);

    // TODO: STL filter?
    for (auto& worker : workers_) {
        if (count == 0)
            break;

        if (worker->state() == Worker::State::STOPPED) {
            worker->start();
            count--;
        }
    }

    while (count--) {
        std::unique_ptr<Worker> worker = std::make_unique<Worker>(request_queue_);
        worker->start();
        workers_.push_back(std::move(worker));
    }
}


void Manager::fire(unsigned int count) {
    std::unique_lock<std::mutex> lock(mtx_);
    request_queue_.addStop(count);
}


void Manager::delegate(std::unique_ptr<Work> work) {
    std::unique_lock<std::mutex> lock(mtx_);

    if (state_ != Manager::State::ACCEPTING)
        throw "Not accepting new works";

    request_queue_.addWork(std::move(work));
}


void Manager::stopAll() {
    std::unique_lock<std::mutex> lock(mtx_);

    request_queue_.setToAlwaysStop(true);

    for (auto& worker : workers_)
        worker->awaitStop();

    workers_.clear();
    request_queue_.resetStopCount();
    request_queue_.setToAlwaysStop(false);
}


std::size_t Manager::workerCount() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return workers_.size();
}


} // namespace work

} // namespace tpool