#ifndef TPOOL_WORK_REQUEST_QUEUE_HPP_
#define TPOOL_WORK_REQUEST_QUEUE_HPP_

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "tpool/work/work.hpp"
#include "tpool/work/request.hpp"

namespace tpool {

namespace work {

class RequestQueue {
public:

    RequestQueue();

    RequestQueue(const RequestQueue&) = delete;

    RequestQueue(RequestQueue&&) noexcept = default;

    RequestQueue& operator=(const RequestQueue&) = delete;

    RequestQueue& operator=(RequestQueue&&) noexcept = default;

    std::size_t stopCount() const;

    std::size_t workCount() const;

    void addStop(std::size_t count);

    void addWork(std::unique_ptr<Work> work);

    Request waitRequest();

private:
    std::queue<std::unique_ptr<Work>> work_queue_;
    std::size_t stop_count_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
    int some_value = 0;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_REQUEST_QUEUE_HPP_