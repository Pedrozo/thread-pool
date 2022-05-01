#ifndef TPOOL_FIXED_POOL_HPP_
#define TPOOL_FIXED_POOL_HPP_

#include <future>

#include "tpool/work/manager.hpp"
#include "tpool/work/future_work.hpp"
#include "tpool/work/stealer_work.hpp"

namespace tpool {

class FixedPool {
public:

    explicit FixedPool(std::size_t size) : size_(size), manager_() {
        manager_.hire(size);
    }

    template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args...>>
    std::future<Ret> submit(Func&& func, Args... args) {
        work::FutureWork<Ret, Args...> work(std::forward<Func>(func), std::forward<Args>(args)...);
        std::future<Ret> fut = work.getFuture();
        manager_.delegate(std::move(work));
        return fut;
    }

    void shutdown() {
        manager_.stopAll();
    }

    void awaitTermination() {
        manager_.awaitAllStop();
    }

private:
    std::size_t size_;
    work::Manager manager_;
};


class WorkStealingPool {
public:

    explicit WorkStealingPool(std::size_t size) : size_(size), manager_() {
        manager_.hire(size);
    }

    template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args...>>
    std::future<Ret> submit(Func&& func, Args... args) {
        work::StealerWork<Ret, Args...> work(manager_.workQueue(), std::forward<Func>(func), std::forward<Args>(args)...);
        std::future<Ret> fut = work.getFuture();
        manager_.delegate(std::move(work));
        return fut;
    }

private:
    std::size_t size_;
    work::Manager manager_;
};


class CachedPool {
public:

    template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args...>>
    std::future<Ret> submit(Func&& func, Args... args) {
        work::FutureWork<Ret, Args...> work(std::forward<Func>(func), std::forward<Args>(args)...);
        std::future<Ret> fut = work.getFuture();
        work_queue_.offer(std::move(work));

        std::unique_lock<std::mutex> lock(mtx_);
        for (auto& worker : workers) {
            if (worker->state() == work::Worker::State::STOPPED)

            if (worker->notify())
                return fut;
        }

        workers_.push_back(std::make_unique<work::Worker>(0, work_queue_, stop_counter_));

        return fut;
    }

private:
    work::SafeQueue<work::Work> work_queue_;
    BoundedCounter<int> stop_counter_;
    std::list<std::unique_ptr<work::Worker>> workers_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

/*
Should the Manager be removed?

policies:
WorkFactory
    * DefaultWork or StealerWork

WorkManager:
    WorkerFactory
    WorkDelegation

State

shutdown and shutdownNow


ThreadPool<WorkFactory, Manager<WorkDelegation, WorkerFactory>>

FixedPool = ThreadPool<DefaultWorkFactory, Manager<DefaultDelegation, CommonWorkerFactory>>
CachedPool = ThreadPool<DefaultWorkFactory, Manager<CachedDelegation, TimedWorkerFactory>>
WorkStealing = ThreadPool<DefaultWorkFactory, Manager<DefaultDelegation, CommonWorkerFactory>>
*/

} // namespace tpool

#endif // TPOOL_FIXED_POOL_HPP_