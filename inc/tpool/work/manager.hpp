#ifndef TPOOL_WORK_MANAGER_HPP_
#define TPOOL_WORK_MANAGER_HPP_

#include <memory>
#include <list>
#include <mutex>
#include <condition_variable>

#include "tpool/work/work.hpp"
#include "tpool/work/worker.hpp"
#include "tpool/work/request.hpp"
#include "tpool/work/request_queue.hpp"

namespace tpool {

namespace work {

class Manager {
public:
	
	~Manager();

	void hire(unsigned int count);

	void fire(unsigned int count);

	void delegate(std::unique_ptr<Work> work);

	std::size_t workerCount() const;

private:
	RequestQueue request_queue_;
	std::list<std::unique_ptr<Worker>> workers_;
	mutable std::mutex mtx_;
	std::condition_variable cond_;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_MANAGER_HPP_