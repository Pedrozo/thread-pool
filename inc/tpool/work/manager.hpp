#ifndef TPOOL_WORK_MANAGER_HPP_
#define TPOOL_WORK_MANAGER_HPP_

#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "tpool/work/work.hpp"
#include "tpool/work/worker.hpp"
#include "tpool/work/instruction.hpp"
#include "tpool/work/instruction_queue.hpp"

namespace tpool {

namespace work {

class Manager {
public:
	
	~Manager();

	void hire(unsigned int count);

	void fire(unsigned int count);

	void pushWork(std::unique_ptr<Work> work); // TODO: better name

	std::size_t workerCount() const;

private:
	InstructionQueue instruction_queue_;
	std::vector<std::unique_ptr<Worker>> workers_; // TODO: use correct data structure
	mutable std::mutex mtx_;
	std::condition_variable cond_;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_MANAGER_HPP_