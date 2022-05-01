#ifndef TPOOL_WORK_POLICIES_DEFAULT_WORKING_POLICY_HPP_
#define TPOOL_WORK_POLICIES_DEFAULT_WORKING_POLICY_HPP_

#include <memory>
#include <mutex>
#include <list>

#include "tpool/util/safe_queue.hpp"
#include "tpool/util/bounded_counter.hpp"
#include "tpool/work/Work.hpp"
#include "tpool/worker/Worker.hpp"

namespace tpool {

namespace work {

namespace policies {

/**
 * A straight-forward working policy, which pushes works to a single work queue
 * consumed by a group of workers. In addition, all its operations are thread-safe.
 */
class DefaultWorkingPolicy {
public:

	/**
	 * Constructs a working policy with the given limit for workers.
	 */
	explicit DefaultWorkingPolicy(std::size_t max_size)
		: max_size_(max_size), work_queue_(std::make_unique<util::SafeQueue<work::Work>>()),
		stop_counter_(0, 0, static_cast<int>(1e6)), workers_(), mtx_() {}

	DefaultWorkingPolicy(const DefaultWorkingPolicy&) = delete;

	DefaultWorkingPolicy(DefaultWorkingPolicy&& other) noexcept
		: max_size_(other.max_size_), work_queue_(std::move(other.work_queue_)),
		stop_counter_(0, 0, static_cast<int>(1e6)), workers_(std::move(workers_)), mtx_() {}

	DefaultWorkingPolicy& operator=(const DefaultWorkingPolicy&) = delete;

	DefaultWorkingPolicy& operator=(DefaultWorkingPolicy&& other) noexcept {
		using std::swap;

		std::unique_lock<std::mutex> lock(mtx_);

		swap(max_size_, other.max_size_);
		swap(work_queue_, other.work_queue_);
		swap(workers_, other.workers_);

		return *this;
	}

	/**
	 * Returns the internal work queue.
	 */
	const util::SafeQueue<work::Work>& work_queue() const {
		return *work_queue_;
	}

	/**
	 * Returns the internal work queue.
	 */
	util::SafeQueue<work::Work>& work_queue() {
		return *work_queue_;
	}

	/**
	 * Adds a work into the work queue to be done by one of the workers. A new worker is created if all workers
	 * are busy and the limit amount of workers has not been reached.
	 */
	void submit(work::Work work) {
		work_queue_->offer(std::move(work));
		std::unique_lock<std::mutex> lock(mtx_);

		if (workers_.size() < max_size_) {
			auto worker = std::make_unique<worker::Worker>(static_cast<unsigned int>(workers_.size()), *work_queue_, stop_counter_);
			worker->start();

			workers_.push_back(std::move(worker));
		}

		for (auto& worker : workers_)
			if (worker->notify())
				break;
	}

private:
	std::size_t max_size_;
	std::unique_ptr<util::SafeQueue<work::Work>> work_queue_;
	util::BoundedCounter<int> stop_counter_;
	std::list<std::unique_ptr<worker::Worker>> workers_;
	mutable std::mutex mtx_;
};

} // namespace policies

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_POLICIES_DEFAULT_WORKING_POLICY_HPP_