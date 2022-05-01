#ifndef TPOOL_UTIL_BOUNDED_COUNTER_HPP_
#define TPOOL_UTIL_BOUNDED_COUNTER_HPP_

#include <mutex>

namespace tpool {

namespace util {

template<typename Counter>
class BoundedCounter {
public:

    // TODO: template metaprogramming: default constructor only when the type has one
    // TODO: template metaprogramming: operators (+=, -=, +) when the type has the operator
    // TODO: builder design pattern

    BoundedCounter(Counter counter, Counter lower, Counter upper)
        : counter_(counter), lower_bound_(lower), upper_bound_(upper), mtx_() {}

    BoundedCounter(const BoundedCounter& other)
        : BoundedCounter(other, std::lock_guard<std::mutex>(other.mtx_)) {}

    BoundedCounter(BoundedCounter&& other) noexcept
        : counter_(std::move(other.counter_)),
        lower_bound_(std::move(other.lower_bound_)),
        upper_bound_(std::move(other.upper_bound_)),
        mtx_() {}

    BoundedCounter& operator=(const BoundedCounter& other) {
        BoundedCounter copy = BoundedCounter(other);
        std::scoped_lock lock(mtx_, other.mtx_);
        *this = std::move(copy);
        return *this;
    }

    BoundedCounter& operator=(BoundedCounter&& other) noexcept {
        std::lock_guard<std::mutex> lock(mtx_);
        counter_ = std::move(other.counter_);
        lower_bound_ = std::move(other.lower_bound_);
        upper_bound_ = std::move(other.upper_bound_);
        return *this;
    }

    bool operator++() {
        std::unique_lock<std::mutex> lock(mtx_);

        if (counter_ == upper_bound_)
            return false;

        counter_++;
        return true;
    }

    bool operator--() {
        std::unique_lock<std::mutex> lock(mtx_);

        if (counter_ == lower_bound_)
            return false;

        counter_--;
        return true;
    }

    Counter& operator*() {
        std::unique_lock<std::mutex> lock(mtx_);
        return counter_;
    }

private:
    BoundedCounter(const BoundedCounter& other, const std::lock_guard<std::mutex>& lock)
        : counter_(other.counter_), lower_bound_(other.lower_bound_),
        upper_bound_(other.upper_bound_), mtx_() {}

    Counter counter_;
    Counter lower_bound_;
    Counter upper_bound_;
    mutable std::mutex mtx_;
};

} // namespace util

} // namespace tpool

#endif // TPOOL_UTIL_BOUNDED_COUNTER_HPP_