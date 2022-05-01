#include <iostream>
#include <cmath>
#include <functional>
#include <future>
#include <chrono>

#include "tpool/thread_pool.hpp"
#include "tpool/work/future_work.hpp"
#include "tpool/work/factories/future_work_factory.hpp"
#include "tpool/work/factories/stealer_work_factory.hpp"
// #include "tpool/worker/manager.hpp"

template<typename Executor>
int fib(Executor& executor, int n) {
    if (n <= 1)
        return n;

    std::future<int> p1 = executor(fib<Executor>, std::ref(executor), n - 1);
    std::future<int> p2 = executor(fib<Executor>, std::ref(executor), n - 2);

    return p1.get() + p2.get();
}

struct BenchmarkResults {
    uint32_t n;
    double mean;
    double std;
};

template<typename Func>
void benchmark(Func& func) {
    uint32_t tests = 20;
    std::vector<uint32_t> times;

    for (uint32_t t = 1; t <= tests; t++) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto stop = std::chrono::high_resolution_clock::now();
        uint32_t dur = (uint32_t) std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        times.push_back(dur);

        std::cout << "#" << t << ": " << dur << "ms" << std::endl;
    }

    double mean = 0.0, std = 0.0;

    for (uint32_t t : times)
        mean += t;
    mean /= tests;

    for (uint32_t t : times)
        std += (t - mean) * (t - mean);

    std /= tests - 1;
    std = sqrt(std);

    std::cout << std::endl;
    std::cout << "mean = " << mean << "ms" << std::endl;
    std::cout << "std = " << std << "ms" << std::endl;

    // return BenchmarkResults { .n = tests, .mean = mean, .std = std };
}

int n = 23;
int dano[50];
int peso[50];

int knap(int proj, int rest) {
    if (proj >= n) {
        return 0;
    }

    int p1 = 0, p2 = 0;

    p1 = knap(proj + 1, rest);

    if (rest >= peso[proj])
        p2 = knap(proj + 1, rest - peso[proj]) + dano[proj];

    return std::max(p1, p2);
}

int main() {
    constexpr std::size_t POOL_SIZE = 12;
    constexpr std::size_t TASKS = static_cast<int>(100);

    auto pool = tpool::makeFixedPool(POOL_SIZE);
    // auto pool = tpool::makeWorkStealingPool(POOL_SIZE);
    // auto& pool = std::async;
    double sum = 0.0;

    // auto sqrt_task = [] (double n) {
    //     return std::sqrt(n);
    // };

    auto task = [&] {
        std::vector<std::future<int>> results;

        for (std::size_t i = 0; i < TASKS; i++)
            results.push_back(std::async(knap, 0, 1000));
            // results.push_back(pool(knap, 0, 1000));

        for (auto& r : results)
            sum += r.get();
    };

    for (int i = 0; i < 50; i++) {
        dano[i] = i * 3 - 2;
        peso[i] = i;
    }

    // auto some_task = [] {
    //     knap(0, 10000);
    // };

    benchmark(task);

    // std::cout << sum << std::endl;


    // double sums = 0

    // auto pool = tpool::makeFixedPool(4);
    // auto pool = tpool::makeWorkStealingPool(4);

    //std::cout << pool(dummy_work).get() << std::endl;

    // tpool::WorkStealingPool pool(tpool::DefaultWorkingPolicy(4));
    // tpool::FixedPool pool(tpool::DefaultWorkingPolicy(4));

    // std::cout << pool(dummy_work).get() << std::endl
    // std::cout << sizeof(pool) << std::endl;

    // std::cout << fib(pool, 12) << std::endl;

    /*
    Foo f;
    pool(byRef, Foo());
    */

    // pool(byValue, Foo());

    /*
    std::future<int> f = pool(dummy_work);

    std::cout << f.get() << std::endl;
    */

    /*
    tpool::util::SafeQueue<tpool::work::Work> work_queue;
    tpool::ThreadPool pool{ tpool::work::factories::StealerWorkFactory(work_queue), LazyPolicy() };

    auto x = pool(dummy_work);
    std::cout << x.get() << std::endl;
    */

    /*
    tpool::Manager<
        decltype(&tpool::defaultWorkerFactory),
        decltype(&tpool::enqueueWorkDelegation)> manager(4, tpool::defaultWorkerFactory, tpool::enqueueWorkDelegation);
        */

    // auto other = std::move(manager);
    // auto other = std::move(b);

    // tpool::ThreadPool<tpool::RegularWorkFactory, decltype(manager)> pool
    // { tpool::RegularWorkFactory(), std::move(manager) };
    return 0;
}
