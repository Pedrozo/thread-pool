#ifndef TPOOL_WORK2_MANAGER_HPP_
#define TPOOL_WORK2_MANAGER_HPP_

#include <list>
#include <mutex>
#include <condition_variable>

#include "tpool/work2/worker.hpp"
#include "tpool/work2/safe_queue.hpp"

namespace tpool {

namespace work2 {

class Manager {
public:

    Manager();

    ~Manager();

    void hire(unsigned int count);

    void fire(unsigned int count);

    void delegate(std::unique_ptr<work::Work> work);

private:
    SafeQueue<std::unique_ptr<work::Work>> work_queue_;
    std::list<std::unique_ptr<Worker>> workers_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

}

}

#endif // TPOOL_WORK2_MANAGER_HPP_