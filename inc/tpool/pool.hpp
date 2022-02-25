#ifndef TPOOL_POOL_HPP_
#define TPOOL_POOL_HPP_

#include <functional>
#include <mutex>
#include <queue>
#include <vector>
#include <memory>
#include <atomic>
#include "work_queue.hpp"
#include "work.hpp"

namespace tpool {

class WorkConsumer {
public:
    
    WorkConsumer(WorkQueue& work_queue) 
        : th_(), work_queue_(work_queue), terminate_(false), working_(false) {
        th_ = std::thread(&WorkConsumer::looper, this);
    }

    ~WorkConsumer() {
        terminate();
    }

    WorkConsumer(const WorkConsumer&) = delete;

    WorkConsumer(WorkConsumer&&) = delete;

    WorkConsumer& operator=(const WorkConsumer&) = delete;

    WorkConsumer& operator=(WorkConsumer&&) = delete;
    
    bool isWorking() const {
        return working_;
    }

    void terminate() {
        std::lock_guard<std::mutex> lock(mtx_);
        terminate_ = true;
        if (th_.joinable())
            th_.join();
    }

private:
    void looper() { // TODO: better name
        while (!terminate_) {
            std::unique_ptr<Work> work = work_queue_.wait();
            working_ = true;
            work->doWork();
            working_ = false;
        }
    }

    /*
    enum class State {
        LOOPING,
        TERMINATING,
        STOPPED
    };
    */

    std::thread th_;
    WorkQueue& work_queue_;
    std::atomic_bool terminate_;
    std::atomic_bool working_;
    std::mutex mtx_;
    // std::atomic<State> state_;
};

class FixedSizePool {
public:
    FixedSizePool(std::size_t size) : size_(size) {}

    template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args&&...>>
    std::future<Ret> submit(Func&& func, Args&&... args) {
        auto work = std::make_unique<FutureWork<Ret, Args...>>(std::forward<Func>(func), std::forward<Args>(args)...);
        auto work_future = work->getFuture();
        std::lock_guard<std::mutex> lock(mtx_);

        if (workers_.size() < size_)
            workers_.push_back(std::make_unique<WorkConsumer>(work_queue_)); // TODO: std::ref???

        work_queue_.push(std::move(work));
        return work_future;
    }

private:
    std::size_t size_;
    std::vector<std::unique_ptr<WorkConsumer>> workers_;
    WorkQueue work_queue_;
    mutable std::mutex mtx_;
};

}

#endif // TPOOL_POOL_HPP_