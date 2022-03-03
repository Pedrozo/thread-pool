#ifndef TPOOL_WORK_REQUEST_HPP_
#define TPOOL_WORK_REQUEST_HPP_

#include <memory>

#include "tpool/work/work.hpp"

namespace tpool {

namespace work {

class Request {
public:

    explicit Request(std::unique_ptr<Work> work) noexcept : work_(std::move(work)) {}

    Request(const Request&) = delete;

    Request(Request&&) noexcept = default;

    Request& operator=(const Request&) = delete;

    Request& operator=(Request&&) noexcept = default;

    bool shouldStop() const noexcept {
        return work_ == nullptr;
    }

    bool shouldWork() const noexcept {
        return work_ != nullptr;
    }

    Work& getWork() {
        return *work_;
    }

private:
    std::unique_ptr<Work> work_;
};

inline Request makeStopRequest() {
    return Request(nullptr);
}

inline Request makeWorkRequest(std::unique_ptr<Work> work) {
    return Request(std::move(work));
}

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_REQUEST_HPP_