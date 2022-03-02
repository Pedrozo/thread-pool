#ifndef TPOOL_WORK_INSTRUCTION_QUEUE_HPP_
#define TPOOL_WORK_INSTRUCTION_QUEUE_HPP_

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "tpool/work/work.hpp"
#include "tpool/work/instruction.hpp"

namespace tpool {

namespace work {

class InstructionQueue {
public:

    // TODO: factory for "work" and "stop" instructions
    InstructionQueue();

    InstructionQueue(const InstructionQueue&) = delete;

    InstructionQueue(InstructionQueue&&) = default;

    InstructionQueue& operator=(const InstructionQueue&) = delete;

    InstructionQueue& operator=(InstructionQueue&&) = default;

    std::size_t stopCount() const;

    std::size_t workCount() const;

    void addStop(std::size_t count);

    void addWork(std::unique_ptr<Work> work);

    Instruction waitInstruction();

private:
    std::queue<std::unique_ptr<Work>> work_queue_;
    std::size_t stop_count_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
    int some_value = 0;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_INSTRUCTION_QUEUE_HPP_