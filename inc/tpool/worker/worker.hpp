#ifndef TPOOL_WORKER_WORKER_HPP_
#define TPOOL_WORKER_WORKER_HPP_

#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>
#include <sstream>

#include "tpool/work/work.hpp"
#include "tpool/util/safe_queue.hpp"
#include "tpool/util/bounded_counter.hpp"

namespace tpool {

namespace worker {

/**
 * A worker that starts an internal thread that continuously executes works.
 * These works can be directly given to it, or then pulled from a shared work queue.
 * Also, worker objects are thread-safe.
 */
class Worker {
public:

    enum class State {
        STOPPED,
        INITIALIZING,
        WAITING,
        NOTIFIED,
        WORKING
    };

    /**
     * Constructs a worker with the given id. It will pull works from the given work queue.
     * Call start() after constructing the worker to start pulling works.
     *
     * @param id this worker id
     * @param shared_queue the work queue that is going to have its works pulled and executed by this worker
     * @param stop_counter a counter used to shutdown workers
     */
    Worker(unsigned int id, util::SafeQueue<work::Work>& shared_queue, util::BoundedCounter<int>& stop_counter);

    Worker(const Worker&) = delete;

    Worker(Worker&&) = delete;

    Worker& operator=(const Worker&) = delete;

    Worker& operator=(Worker&&) = delete;

    ~Worker();

    /**
     * Returns the worker current state.
     */
    State state() const;

    /**
     * Request this worker to execute the given work.
     *
     * @param work the work to be performed
     * @throws std::string if the worker is already busy with another work
     */
    void doWork(work::Work work);

    /**
     * Starts the worker so it can pull works from the shared work queue.
     *
     * @throws std::string if the worker is already started
     */
    void start();

    /**
     * Requests the worker to stop. If it is doing some work at the moment,
     * it will finish the work before shutting down.
     *
     * @throws std::string if the worker is already stopped
     */
    void stop();

    /**
     * Waits until the worker stop. Do not forget to call stop() before calling this,
     * otherwise you might wait forever (unless stop() is called from another thread).
     */
    void awaitStop();

    /**
     * Notifies this worker to check if a new work (from a shared work queue, or directly by doWork()) is available to be performed.
     *
     * @retun true if the notification has changed the worker state from WAITING to NOTIFIED
     */
    bool notify();

private:

    void loop();

    unsigned int id_;
    State state_;
    std::thread thr_;
    bool stop_;
    std::optional<work::Work> next_work_;
    util::SafeQueue<work::Work>& shared_queue_;
    util::BoundedCounter<int>& stop_counter_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

template<typename First, typename ... T>
inline bool isAnyOf(First&& first, T&& ... t) {
    return ((first == t) || ...);
}

} // namespace worker

} // namespace tpool

#endif // TPOOL_WORK_WORKER_HPP_