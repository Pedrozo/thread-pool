#ifndef TPOOL_WORK_FACTORIES_FUTURE_WORK_FACTORY_HPP_
#define TPOOL_WORK_FACTORIES_FUTURE_WORK_FACTORY_HPP_

#include <utility>
#include <future>

#include "tpool/work/future_work.hpp"

namespace tpool {

namespace work {

namespace factories {


/**
 * Factory of FutureWork objects
 */
struct FutureWorkFactory {

    template<typename Ret, typename... Args>
    using CreatedType = FutureWork<Ret, Args...>;

    /**
     * Creates a FutureWork from the given callable and arguments
     *
     * @param func the callable (a function or a functor) to be wrapped
     * @param args the callable arguments
     * @return a FutureWork wrapping the given callable and arguments
     */
    template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args...>>
    constexpr CreatedType<Ret, Args...> makeWork(Func&& func, Args... args) const {
        return FutureWork<Ret, Args...>(std::forward<Func>(func), std::forward<Args>(args)...);
    }
};

} // namespace factories

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_FACTORIES_FUTURE_WORK_FACTORY_HPP_