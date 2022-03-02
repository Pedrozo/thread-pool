#ifndef TPOOL_WORK_REQUEST_HPP_
#define TPOOL_WORK_REQUEST_HPP_

#include <memory>

#include "tpool/work/work.hpp"

namespace tpool {

namespace work {

class Request {
public:

    explicit Request(std::unique_ptr<Work> work) : work_(std::move(work)) {}

    Request(const Request&) = delete;

    Request(Request&&) = default;

    Request& operator=(const Request&) = delete;

    Request& operator=(Request&&) = default;

    bool shouldStop() const {
        return work_ == nullptr;
    }

    bool shouldWork() const {
        return work_ != nullptr;
    }

    Work& getWork() {
        return *work_;
    }

private:
    std::unique_ptr<Work> work_;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_REQUEST_HPP_