#ifndef TPOOL_WORK_FUTURE_WORK_HPP_
#define TPOOL_WORK_FUTURE_WORK_HPP_

#include <utility>
#include <tuple>
#include <future>

#include "work.hpp"

namespace tpool {

namespace work {

/**
 * Wraps a callable object along with its arguments. One can invoke this work so it
 * runs the internal callable with the given arguments, and stores the callable result
 * (or exception) in std::future objects.
 */
template<typename Ret, typename... Args>
class FutureWork {
public:

    /**
     * Constructs a FutureWork.
     *
     * @param func the callable (e.g. function, lambda expression, bind expression, functor)
     * @param args the arguments to be forwarded to the callable
     */
    template<typename Func>
    FutureWork(Func&& func, Args... args)
        : task_(std::forward<Func>(func)), args_(std::forward<Args>(args)...) {}

    FutureWork(const FutureWork&) = delete;

    FutureWork(FutureWork&&) = default;

    FutureWork& operator=(const FutureWork&) = delete;

    FutureWork& operator=(FutureWork&&) = default;

    /**
     * Returns a std::future associated with wrapped callable
     */
    std::future<Ret> getFuture() {
        return task_.get_future();
    }

    /**
     * Invokes the callable with its arguments, and stores its
     * result in the associated std::future objects.
     */
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