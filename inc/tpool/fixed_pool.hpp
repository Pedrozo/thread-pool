#ifndef TPOOL_FIXED_POOL_HPP_
#define TPOOL_FIXED_POOL_HPP_

#include <future>

#include "tpool/work/manager.hpp"

namespace tpool {

class FixedPool {
public:

    enum class State {
        SHUTDOWN,
        REFUSING,
        ACCEPTING
    };

    explicit FixedPool(std::size_t size) : size_(size), manager_() {
        manager_.hire(size);
    }

    template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args...>>
    std::future<Ret> submit(Func&& func, Args... args) {
        work::FutureWork<Ret, Args...> f_work(std::forward<Func>(func), std::forward<Args>(args)...);
        std::future<Ret> fut = f_work.getFuture();
        std::unique_ptr<work::Work> work = std::make_unique<work::FutureWork<Ret, Args...>>(std::move(f_work));
        manager_.delegate(std::move(work));
        return fut;
    }

private:
    std::size_t size_;
    State state_;
    work::Manager manager_;
};

} // namespace tpool

#endif // TPOOL_FIXED_POOL_HPP_