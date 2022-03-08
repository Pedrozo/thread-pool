#ifndef TPOOL_WORK_WORKER_HPP_
#define TPOOL_WORK_WORKER_HPP_

#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "tpool/work/work.hpp"
#include "tpool/work/safe_queue.hpp"
#include "tpool/work/bounded_counter.hpp"

namespace tpool {

namespace work {

class Worker {
public:

    enum class State {
        STOPPED,
        INITIALIZING,
        WAITING,
        NOTIFIED,
        WORKING
    };

    explicit Worker(SafeQueue<Work>& shared_queue, BoundedCounter<int>& stop_counter);

    Worker(const Worker&) = delete;

    Worker(Worker&&) = delete;

    Worker& operator=(const Worker&) = delete;

    Worker& operator=(Worker&&) = delete;

    ~Worker();

    State state() const;

    void doWork(Work work);

    void start();

    void stop();

    void awaitStop();

    bool notify();

private:

    void loop();

    State state_;
    std::thread thr_;
    bool stop_;
    std::optional<Work> next_work_;
    SafeQueue<Work>& shared_queue_;
    BoundedCounter<int>& stop_counter_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

template<typename First, typename ... T>
inline bool isAnyOf(First&& first, T&& ... t) {
    return ((first == t) || ...);
}

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_WORKER_HPP_