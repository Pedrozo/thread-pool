#ifndef TPOOL_WORK_FACTORIES_STEALER_WORK_FACTORY_HPP_
#define TPOOL_WORK_FACTORIES_STEALER_WORK_FACTORY_HPP_

#include <utility>

#include "tpool/util/safe_queue.hpp"
#include "tpool/work/stealer_work.hpp"

namespace tpool {

namespace work {

namespace factories {

/**
 * Factory of StealerWork objects
 */
class StealerWorkFactory {
public:

    template<typename Ret, typename... Args>
    using CreatedType = StealerWork<Ret, Args...>;

    explicit StealerWorkFactory(util::SafeQueue<work::Work>& work_queue) : work_queue_(work_queue) {}

    /**
     * Creates a StealerWork from the given callable and arguments
     *
     * @param func the callable (a function or a functor) to be wrapped
     * @param args the callable arguments
     * @return a StealerWork wrapping the given callable and arguments
     */
    template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args...>>
    constexpr CreatedType<Ret, Args...> makeWork(Func&& func, Args... args) {
        return StealerWork<Ret, Args...>(work_queue_, std::forward<Func>(func), std::forward<Args>(args)...);
    }

private:
    util::SafeQueue<work::Work>& work_queue_;
};

} // namespace factories

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_FACTORIES_STEALER_WORK_FACTORY_HPP_