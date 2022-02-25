#ifndef TPOOL_WORK_INSTRUCTION_HPP_
#define TPOOL_WORK_INSTRUCTION_HPP_

#include <memory>

#include "tpool/work/work.hpp"

namespace tpool {

namespace work {

class Instruction {
public:

    explicit Instruction(std::unique_ptr<Work> work) : work_(std::move(work)) {}

    Instruction(const Instruction&) = delete;

    Instruction(Instruction&&) = default;

    Instruction& operator=(const Instruction&) = delete;

    Instruction& operator=(Instruction&&) = default;

    bool isStop() const {
        return work_ == nullptr;
    }

    bool isWork() const {
        return work_ != nullptr;
    }

    Work& work() {
        return *work_;
    }

private:
    std::unique_ptr<Work> work_;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_INSTRUCTION_HPP_