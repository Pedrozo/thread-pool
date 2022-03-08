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
#include "tpool/work/safe_queue.hpp"
#include "tpool/work/worker.hpp"


using namespace tpool;

template<typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&, Args...>>
std::pair<std::unique_ptr<work::Work>, std::future<Ret>> make_work(Func&& func, Args... args) {
    work::FutureWork<Ret, Args...> fwork(std::forward<Func>(func), std::forward<Args>(args)...);
    std::future<Ret> f = fwork.getFuture();
    return std::make_pair(std::make_unique<work::FutureWork<Ret, Args...>>(std::move(fwork)), std::move(f));
}

void test(work::Work f) {
    std::cout << "running..." << std::endl;
    f();
    std::cout << "finish" << std::endl;
}

void hello() {
    std::cout << "started" << std::endl;
    for (int i = 0; i < 1e9 * 2; i++);
    std::cout << "finished" << std::endl;
}

int main() {
    work::Manager manager;

    manager.hire(4);

    for (int i = 0; i < 1e9; i++);
    std::cout << "------------------------" << std::endl;

    manager.delegate(hello);
    manager.fire(2);

    std::cout << "------------------------" << std::endl;
    for (int i = 0; i < 1e9; i++);

    return 0;
}