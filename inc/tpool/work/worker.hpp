#ifndef TPOOL_WORK_WORKER_HPP_
#define TPOOL_WORK_WORKER_HPP_

#include <thread>
#include <memory>
#include <mutex>

#include "tpool/work/instruction_queue.hpp"

namespace tpool {

namespace work {

class Worker {
public:

    enum class State {
        STOPPED,
        WAITING,
        WORKING
    };

    explicit Worker(InstructionQueue& instruction_queue);

    Worker(const Worker&) = delete;

    Worker(Worker&&) = delete;

    Worker& operator=(const Worker&) = delete;

    Worker& operator=(Worker&&) = delete;

    ~Worker();

    State state() const;

    void start();

private:
    void loop();

    State state_; // TODO: check data-race
    std::thread thr_;
    InstructionQueue& instruction_queue_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_WORKER_HPP_