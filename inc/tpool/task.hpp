#ifndef TPOOL_TASK_HPP_
#define TPOOL_TASK_HPP_

#include <type_traits>
#include <future>
#include <functional>

namespace tpool {

/*
class Task {
public:

    template<typename ReturnType, typename Callable, typename... Args>
    Task(std::promise<ReturnType> promise, Callable&& function, Args&&... args)
        : impl_(std::make_unique<Model<ReturnType>>(std::move(promise), std::forward<Callable>(function), std::forward<Args>(args)...)) {}

    template<typename Callable, typename... Args>
    Task(std::promise<void> promise, Callable&& function, Args&&... args)
        : impl_(std::make_unique<Model<void>>(std::move(promise), std::forward<Callable>(function), std::forward<Args>(args)...)) {}

    ~Task() = default;

    Task(const Task&) = delete;

    Task(Task&&) = default;

    Task& operator=(const Task&) = delete;

    Task& operator=(Task&&) = default;

    void doWork() {
        impl_->doWork();
    }

private:
    struct Concept {

        virtual ~Concept() = default;

        virtual void doWork() = 0;
    };

    template<typename ReturnType>
    struct Model : Concept {

        template<typename ReturnType, typename Callable, typename... Args>
        Model(std::promise<ReturnType> promise, Callable&& function, Args&&... args)
            : promise_(std::move(promise)), work_(std::bind(std::forward<Callable>(function), std::forward<Args>(args)...)) {}

        void doWork() override {
            promise_.set_value(work_());
        }

        std::function<ReturnType()> work_;
        std::promise<ReturnType> promise_;
    };

    template<>
    struct Model<void> : Concept {

        template<typename Callable, typename... Args>
        Model(std::promise<void> promise, Callable&& function, Args&&... args)
            : promise_(std::move(promise)), work_(std::bind(std::forward<Callable>(function), std::forward<Args>(args)...)) {}

        void doWork() override {
            work_();
            promise_.set_value();
        }

        std::function<void()> work_;
        std::promise<void> promise_;
    };

    std::unique_ptr<Concept> impl_;
};
*/

// that's almost a packaged_task
template<typename ResultType>
class Task {
public:

    Task(std::function<ResultType()> work)
        : work_(std::move(work)), promise_() {}

    Task(const Task&) = delete;

    Task(Task&&) = default;

    Task& operator=(const Task&) = delete;

    Task& operator=(Task&&) = default;

    void operator()() {
        try {
            promise_.set_value(work_());
        } catch (...) {
            promise_.set_exception(std::current_exception());
        }
    }

    std::future<ResultType> getFuture() {
        return promise_.get_future();
    }

private:
    std::function<ResultType()> work_;
    std::promise<ResultType> promise_;
};

template<>
void Task<void>::operator()() {
    try {
        work_();
        promise_.set_value();
    } catch (...) {
        promise_.set_exception(std::current_exception());
    }
}

template<typename Callable, typename... Args, typename ResultType = std::invoke_result_t<Callable&&, Args&&...>>
Task<ResultType> make_task(Callable&& work, Args&&... args) {
    return Task<ResultType>(std::bind(std::forward<Callable>(work), std::forward<Args>(args)...)); // TODO: is binding really necessary?
}

} // namespace tpool

#endif // TPOOL_TASK_HPP_
