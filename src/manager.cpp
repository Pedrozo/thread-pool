#include "tpool/work/manager.hpp"

namespace tpool {

namespace work {

Manager::~Manager() {
    std::unique_lock<std::mutex> lock(mtx_);
	instruction_queue_.addStop(workers_.size());
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
        std::unique_ptr<Worker> worker = std::make_unique<Worker>(instruction_queue_);
        worker->start();
        workers_.push_back(std::move(worker));
    }
}


void Manager::fire(unsigned int count) {
    std::unique_lock<std::mutex> lock(mtx_);
    instruction_queue_.addStop(count);
}


void Manager::pushWork(std::unique_ptr<Work> work) {
    std::unique_lock<std::mutex> lock(mtx_);
    instruction_queue_.addWork(std::move(work));
}


std::size_t Manager::workerCount() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return workers_.size();
}


} // namespace work

} // namespace tpool