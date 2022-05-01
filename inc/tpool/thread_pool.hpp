#ifndef TPOOL_THREAD_POOL_HPP_
#define TPOOL_THREAD_POOL_HPP_

#include <memory>
#include <functional>
#include <list>

#include "work/work.hpp"
#include "worker/worker.hpp"
#include "util/safe_queue.hpp"
#include "util/bounded_counter.hpp"
#include "work/factories/stealer_work_factory.hpp"
#include "work/factories/future_work_factory.hpp"
#include "work/policies/default_working_policy.hpp"

namespace tpool {

/**
 * A Thread Pool with policy based design. The policies are:
 *   - WorkFactory: Creates a work from a callable and its arguments.
 *   - WorkingPolicy: The works are submitted to a working policy that will 
 *     execute the work usually in a different thread
 */
template<typename WorkFactory, typename WorkingPolicy = work::policies::DefaultWorkingPolicy>
class Pool {
public:

	/**
	 * Construct a thread pool with the given work factory policy and working policy
	 * 
	 * @param work_factory the work factory policy, which defines the type of the works created
	 * @param working_policy the strategy of submitting works to the pool
	 */
	explicit Pool(WorkFactory work_factory = WorkFactory(), WorkingPolicy working_policy = WorkingPolicy())
		: work_factory_(std::move(work_factory)), working_policy_(std::move(working_policy)) {}

	template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args...>>
	std::future<Ret> operator()(Func&& func, Args... args) {
		auto future_work = work_factory_.makeWork(std::forward<Func>(func), std::forward<Args>(args)...);
		auto future = future_work.getFuture();
		working_policy_.submit(std::move(future_work));
		return future;
	}

private:
	WorkFactory work_factory_;
	WorkingPolicy working_policy_;
};

/**
 * A thread pool with a fixed number of threads. The works are executed in the order they are submitted.
 */
using FixedPool = Pool<work::factories::FutureWorkFactory>;

/**
 * A working-stealing thread pool. The works are executed in the order they are submitted.
 * In addition, std::future.get() calls will not block the current thread in order to wait for the result,
 * instead it will steal and do works from the work queue until the work related to this std::future
 * gets done (by the work-stealing or by one of the worker threads).
 */
using WorkStealingPool = Pool<work::factories::StealerWorkFactory>;

/**
 * Creates a thread pool with the given number of threads.
 * 
 * @param size the number of threads
 * @return a FixedPool
 */
inline FixedPool makeFixedPool(std::size_t size) {
	return FixedPool(work::factories::FutureWorkFactory(),
		work::policies::DefaultWorkingPolicy(size));
}

/**
 * Creates a work-stealing thread pool with the given number of threads.
 * 
 * @param size the number of threads
 * @return a WorkStealingPool
 */
inline WorkStealingPool makeWorkStealingPool(std::size_t size) {
	work::policies::DefaultWorkingPolicy working_policy(size);
	work::factories::StealerWorkFactory work_factory(working_policy.work_queue());
	return WorkStealingPool(std::move(work_factory), std::move(working_policy));
}


} // namespace tpool

#endif // TPOOL_THREAD_POOL_HPP_