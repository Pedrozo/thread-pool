#include "tpool/work/worker.hpp"

#include <iostream>

namespace tpool {

namespace work {

Worker::Worker(InstructionQueue& instruction_queue)
    : state_(State::STOPPED), thr_(), instruction_queue_(instruction_queue) {}


Worker::~Worker() {
    // TODO: check race condition
    if (thr_.joinable()) {
        thr_.join();
    }
}


Worker::State Worker::state() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return state_;
}


void Worker::start() {
    std::unique_lock<std::mutex> lock(mtx_);

    if (state_ != Worker::State::STOPPED)
        throw "..."; // TODO: proper exception

    thr_ = std::thread(&Worker::loop, this);
}


void Worker::loop() {
    std::unique_lock<std::mutex> lock(mtx_);

    while (true) {
        state_ = Worker::State::WAITING;

        lock.unlock();
        Instruction inst = instruction_queue_.waitInstruction();
        lock.lock();

        if (inst.isStop())
            break;

        state_ = Worker::State::WORKING;
        lock.unlock();
        inst.work()();
        lock.lock();
    }

    state_ = Worker::State::STOPPED;
}

} // namespace work

} // namespace tpool