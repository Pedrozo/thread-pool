#ifndef TPOOL_WORK_STEALER_WORK_HPP_
#define TPOOL_WORK_STEALER_WORK_HPP_

#include <utility>
#include <tuple>
#include <future>
#include <atomic>
#include <memory>

#include "tpool/work/work.hpp"
#include "tpool/util/safe_queue.hpp"

namespace tpool {

namespace work {

/**
 * Wraps a callable object along with its arguments. One can invoke this work so it
 * runs the internal callable with the given arguments, and stores the callable result
 * (or exception) in std::future objects. In addition, unlike FutureWork, the std::future.get()
 * will steal (and invoke) works from the work queue.
 */
template<typename Ret, typename... Args>
class StealerWork {
public:

    /**
     * Construts a StealWork
     *
     * @param work_queue the work queue to have works stole
     * @param func the callable (e.g. function, lambda expression, bind expression, functor)
     * @param args the arguments to be forwarded to the callable
     */
    template<typename Func>
    StealerWork(util::SafeQueue<Work>& work_queue, Func&& func, Args... args)
        : impl_(std::make_shared<Impl>(work_queue,
                                       std::forward<Func>(func),
                                       std::forward<Args>(args)...)) {
        impl_->setShared(impl_);
    }

    /**
     * Returns a std::future associated with wrapped callable
     */
    std::future<Ret> getFuture() {
        return impl_->getFuture();
    }

    /**
     * Invokes the callable with its arguments, and stores its
     * result in the associated std::future objects.
     */
    void operator()() {
        (*impl_)();
    }

private:

    struct Impl {

        template<typename Func>
        Impl(util::SafeQueue<Work>& work_queue, Func&& func, Args... args)
            : completed_(false), task_(std::forward<Func>(func)), args_(std::forward<Args>(args)...),
              task_future_(task_.get_future()), work_queue_(work_queue), shared_impl_(nullptr) {}

        std::future<Ret> getFuture() {
            return std::async(std::launch::deferred, &Impl::lazy, this, *shared_impl_);
        }

        void setShared(std::shared_ptr<Impl>& impl) {
            shared_impl_ = &impl;
        }

        void operator()() {
            std::apply(task_, args_);
            completed_ = true;
        }

        Ret lazy(std::shared_ptr<Impl> impl) {
            while (!completed_) {
                auto stoled = work_queue_.poll();
                if (stoled)
                    (*stoled)();
            }

            return task_future_.get();
        }

        std::atomic<bool> completed_;
        std::packaged_task<Ret(Args...)> task_;
        std::tuple<Args...> args_;
        std::future<Ret> task_future_;
        util::SafeQueue<Work>& work_queue_;
        std::shared_ptr<Impl> *shared_impl_;
    };

    std::shared_ptr<Impl> impl_;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_STEALER_WORK_HPP_