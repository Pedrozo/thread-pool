#include <iostream>
#include <cmath>
#include <functional>
#include <future>

// #include "tpool/work/work.hpp"
// #include "tpool/work/future_work.hpp"
// #include "tpool/work2/worker.hpp"
// #include "tpool/work2/safe_queue.hpp"
// #include "tpool/work2/manager.hpp"

#include "tpool/fixed_pool.hpp"


using namespace tpool;

template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args...>>
std::pair<std::unique_ptr<work::Work>, std::future<Ret>> make_work(Func&& func, Args... args) {
    work::FutureWork<Ret, Args...> fwork(std::forward<Func>(func), std::forward<Args>(args)...);
    std::future<Ret> f = fwork.getFuture();
    return std::make_pair(std::make_unique<work::FutureWork<Ret, Args...>>(std::move(fwork)), std::move(f));
}

int main() {
    FixedPool pool(16);
    constexpr int SIZE = static_cast<int>(1e6);
    std::vector<std::future<double>> results;
    double sum = 0.;

    for (int i = 0; i < SIZE; i++)
        results.push_back(pool.submit(sqrt, static_cast<double>(i)));

    for (auto& r : results)
        sum += r.get();

    std::cout << sum << std::endl;

    // std::future<double> f = pool.submit(sqrt, 4.);

    // std::cout << f.get() << std::endl;

    /*
    work2::SafeQueue<std::unique_ptr<work::Work>> safe_queue;
    work2::Worker worker(safe_queue);
    std::vector<std::future<double>> results;
    double sum = 0.;

    constexpr int SIZE = static_cast<int>(1e5);

    for (int i = 0; i < SIZE; i++) {
        auto work = make_work(sqrt, static_cast<double>(i));
        safe_queue.push(std::move(work.first));
        results.push_back(std::move(work.second));
    }

    worker.start();

    auto other = make_work(sqrt, 144.);
    worker.doWork(std::move(other.first));

    std::cout << other.second.get() << std::endl;

    for (auto& result : results) {
        sum += result.get();
    }

    std::cout << sum << std::endl;
    */

    // auto work = make_work(sum, 1, 2);

    // worker.doWork(std::move(work.first));

    // std::cout << work.second.get() << std::endl;

    return 0;
}