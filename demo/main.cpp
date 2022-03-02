#include <iostream>

#include "tpool/work/work.hpp"
#include "tpool/work/future_work.hpp"
#include "tpool/work/request.hpp"
#include "tpool/work/request_queue.hpp"
#include "tpool/work/worker.hpp"
#include "tpool/work/manager.hpp"

using namespace tpool;

int main() {
    work::Manager man;
    man.hire(4);    

    man.delegate(std::make_unique<work::FutureWork<void>>([] {
        std::cout << "hello" << std::endl;
    }));

    return 0;
}