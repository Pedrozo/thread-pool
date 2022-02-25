#ifndef TPOOL_WORK_WORK_HPP_
#define TPOOL_WORK_WORK_HPP_

namespace tpool {

namespace work {

class Work {
public:
    virtual ~Work() = default;
    virtual void operator()() = 0;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_WORK_HPP_