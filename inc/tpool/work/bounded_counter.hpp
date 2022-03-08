#ifndef TPOOL_WORK_BOUNDED_COUNTER_HPP_
#define TPOOL_WORK_BOUNDED_COUNTER_HPP_

template<typename Counter>
class BoundedCounter {
public:

    // TODO: template metaprogramming: default constructor only when the type has one
    // TODO: template metaprogramming: operators (+=, -=, +) when the type has the operator
    // TODO: builder design pattern

    BoundedCounter(Counter counter, Counter lower, Counter upper)
        : counter_(counter), lower_bound_(lower), upper_bound_(upper) {}

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
    Counter counter_;
    Counter lower_bound_;
    Counter upper_bound_;
    mutable std::mutex mtx_;
};

#endif // TPOOL_WORK_BOUNDED_COUNTER_HPP_