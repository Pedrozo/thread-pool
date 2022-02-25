#ifndef TPOOL_WORKER_HPP_
#define TPOOL_WORKER_HPP_

#include <thread>
#include <conditional_variable>

namespace tpool {


class Worker {
public:
    Worker() = default;
    ~Worker() = default;

    Worker(const Worker&) = delete;
    Worker(Worker&&) = delete;

    Worker& operator=(const Worker&) = delete;
    Worker& operator=(Worker&&) = delete;

private:
    void loop() {
        while (true) {

        }
    }

    std::thread thr_;
    std::conditional_variable cv_;
};

} // namespace tpool

#endif // TPOOL_WORKER_HPP_