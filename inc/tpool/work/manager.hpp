#ifndef TPOOL_WORK_MANAGER_HPP_
#define TPOOL_WORK_MANAGER_HPP_

#include <list>
#include <mutex>
#include <condition_variable>

#include "tpool/work/worker.hpp"
#include "tpool/work/safe_queue.hpp"

namespace tpool {

namespace work {

class Manager {
public:

    Manager();

    ~Manager();

    void hire(unsigned int count);

    void fire(unsigned int count);

    void delegate(Work work);

private:
    SafeQueue<Work> work_queue_;
    std::list<std::unique_ptr<Worker>> workers_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

}

}

#endif // TPOOL_WORK2_MANAGER_HPP_