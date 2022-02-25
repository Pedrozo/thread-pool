#ifndef TPOOL_WORK_HPP_
#define TPOOL_WORK_HPP_

namespace tpool {

class Work {
public:
    virtual ~Work() = default;
    virtual void doWork() = 0;
};

template<typename Ret, typename... Args>
class FutureWork : public Work {
public:

    template<typename Func>
    FutureWork(Func&& func, Args&&... args)
        : task_(std::forward<Func>(func)), args_(std::forward<Args>(args)...) {}

    std::future<Ret> getFuture() {
        return task_.get_future();
    }

    void doWork() override {
        std::apply(task_, args_); // C++ 17 (https://en.cppreference.com/w/cpp/utility/apply) TODO: make it C++ 14
    }

private:
    std::packaged_task<Ret(Args...)> task_;
    std::tuple<Args...> args_;
};

} // namespace tpool

#endif // TPOOL_WORK_HPP_