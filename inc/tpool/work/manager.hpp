#ifndef TPOOL_WORK_MANAGER_HPP_
#define TPOOL_WORK_MANAGER_HPP_

#include <list>
#include <mutex>
#include <condition_variable>

#include "tpool/worker/worker.hpp"
#include "tpool/util/safe_queue.hpp"

namespace tpool {

namespace work {

class Manager {
public:

    enum class State {
        REFUSING,
        ACCEPTING
    };

    Manager();

    void hire(unsigned int count);

    void fire(unsigned int count);

    void stopAll();

    void awaitAllStop();

    void delegate(Work work);

    util::SafeQueue<Work>& workQueue();

private:
    State state_;
    util::SafeQueue<Work> work_queue_;
    util::BoundedCounter<int> stop_counter_;
    std::list<std::unique_ptr<worker::Worker>> workers_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

}

}

#endif // TPOOL_WORK2_MANAGER_HPP_