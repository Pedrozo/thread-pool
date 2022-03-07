#ifndef TPOOL_WORK_FUTURE_WORK_HPP_
#define TPOOL_WORK_FUTURE_WORK_HPP_

#include <utility>
#include <tuple>
#include <future>

#include "work.hpp"

namespace tpool {

namespace work {

// TODO: template definition like packaged_task
template<typename Ret, typename... Args>
class FutureWork {
public:

    template<typename Func>
    FutureWork(Func&& func, Args... args)
        : task_(std::forward<Func>(func)), args_(std::forward<Args>(args)...) {}

    FutureWork(const FutureWork&) = delete;

    FutureWork(FutureWork&&) = default;

    FutureWork& operator=(const FutureWork&) = delete;

    FutureWork& operator=(FutureWork&&) = default;

    std::future<Ret> getFuture() {
        return task_.get_future();
    }

    void operator()() {
        std::apply(task_, args_); // C++ 17 (https://en.cppreference.com/w/cpp/utility/apply) TODO: make it C++ 14 (or 11)
    }

private:
    std::packaged_task<Ret(Args...)> task_;
    std::tuple<Args...> args_;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_FUTURE_WORK_HPP_