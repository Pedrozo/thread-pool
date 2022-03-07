#ifndef TPOOL_WORK_WORK_HPP_
#define TPOOL_WORK_WORK_HPP_

#include <utility>
#include <memory>

namespace tpool {

namespace work {

class Work {
public:

    template<typename T>
    Work(T&& impl) : impl_(std::make_unique<CallableImpl<T>>(std::forward<T>(impl))) {}

    void operator()() {
        (*impl_)();
    }

private:
    struct Callable {
        virtual ~Callable() = default;
        virtual void operator()() = 0;
    };

    template<typename T>
    struct CallableImpl : public Callable {

        explicit CallableImpl(T&& impl) : impl_(std::forward<T>(impl)) {}

        void operator()() override {
            impl_();
        }

        T impl_;
    };

    std::unique_ptr<Callable> impl_;
};

} // namespace work

} // namespace tpool

#endif // TPOOL_WORK_WORK_HPP_