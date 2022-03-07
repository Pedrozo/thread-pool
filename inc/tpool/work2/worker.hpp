#ifndef TPOOL_WORK2_WORKER_HPP_
#define TPOOL_WORK2_WORKER_HPP_

#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "tpool/work/work.hpp"
#include "tpool/work2/safe_queue.hpp"

namespace tpool {

namespace work2 {

class Worker {
public:

    enum class State {
        STOPPED,
        INITIALIZING,
        WAITING,
        WORKING
    };

    explicit Worker(SafeQueue<std::unique_ptr<work::Work>>& shared_queue);

    Worker(const Worker&) = delete;

    Worker(Worker&&) = delete;

    Worker& operator=(const Worker&) = delete;

    Worker& operator=(Worker&&) = delete;

    ~Worker();

    State state() const;

    void doWork(std::unique_ptr<work::Work> work);

    void start();

    void stop();

    void awaitStop();

    bool notify();

private:

    void loop();

    State state_;
    std::thread thr_;
    std::unique_ptr<work::Work> next_work_;
    bool stop_;
    SafeQueue<std::unique_ptr<work::Work>>& shared_queue_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_WORKER_HPP_