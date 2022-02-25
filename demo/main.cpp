#include <iostream>
#include <memory>
#include <future>

//#include "tpool/work/work.hpp"
// #include "tpool/work/future_work.hpp"

void say_hello() {
    std::cout << "hello!" << std::endl;
}

static void print_num(int x) {
    std::cout << "num = " << x << std::endl;
}

// using namespace tpool;

int main() {
    // std::unique_ptr<work::Work> some_work = std::make_unique<work::FutureWork<void>>(say_hello);
    // (*some_work)();

    // std::unique_ptr<work::Work> other_work = std::make_unique<work::FutureWork<void, int>>(print_num, 100);
    // (*other_work)();

    // work::FutureWork<void, int> alface(print_num, 100);
    // alface();

    std::cout << "mais que poha" << std::endl;
    std::packaged_task<void(int)> mano(print_num);
    mano(100);

    return 0;
}