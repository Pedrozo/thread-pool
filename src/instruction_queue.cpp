#include "tpool/work/instruction_queue.hpp"

namespace tpool {

namespace work {

InstructionQueue::InstructionQueue() : work_queue_(), stop_count_(0), mtx_(), cond_() {}

std::size_t InstructionQueue::stopCount() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return stop_count_;
}


std::size_t InstructionQueue::workCount() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return work_queue_.size();
}


void InstructionQueue::addStop(std::size_t count) {
    std::unique_lock<std::mutex> lock(mtx_);
    stop_count_ += count;

    lock.unlock();

    if (count > 1)
        cond_.notify_all();
    else
        cond_.notify_one();
}


void InstructionQueue::addWork(std::unique_ptr<Work> work) {
    std::unique_lock<std::mutex> lock(mtx_);
    work_queue_.push(std::move(work));
    lock.unlock();
    cond_.notify_one();
}


Instruction InstructionQueue::waitInstruction() {
    std::unique_lock<std::mutex> lock(mtx_);

    cond_.wait(lock, [&] {
        return stop_count_ > 0 || work_queue_.size() > 0;
    });

    if (work_queue_.size() > 0) {
        Instruction inst(std::move(work_queue_.front()));
        work_queue_.pop();
        return inst;
    } else {
        --stop_count_;
        return Instruction(nullptr);
    }
}


} // namespace work

} // namespace tpool
