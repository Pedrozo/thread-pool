#ifndef TPOOL_WORKER_MANAGER_HPP_
#define TPOOL_WORKER_MANAGER_HPP_

#include <list>

#include "tpool/util/bounded_counter.hpp"
#include "tpool/util/safe_queue.hpp"
#include "tpool/work/work.hpp"
#include "tpool/worker/worker.hpp"

namespace tpool {

namespace worker {

template<typename WorkerFactory, typename DelegationPolicy>
class Manager : private WorkerFactory, DelegationPolicy {
    // using WorkQueue = util::SafeQueue<work::Work>;
    // using StopCounter = util::BoundedCounter<int>;

public:
    Manager(std::size_t initial_worker_count)
        : work_queue_(), stop_counter_(0, 0, static_cast<int>(1e6)), workers_() {}

    void submit(work::Work work) {
        // delegation_policy_(std::move(work));
    }

private:
    WorkQueue work_queue_;
    StopCounter stop_counter_;
    std::list<Worker> workers_;
    // WorkerFactory worker_factory_;
    // DelegationPolicy delegation_policy_;
};

} // namespace worker

} // namespace tpool

#endif // TPOOL_WORKER_MANAGER_HPP_