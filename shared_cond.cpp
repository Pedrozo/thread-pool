#include <bits/stdc++.h>

class Work {
public:
    virtual ~Work() = default;
    virtual void operator()() = 0;
};

template<typename Ret, typename... Args>
class FutureWork : public Work {
public:

    template<typename Func>
    FutureWork(Func&& func, Args... args)
        : task_(std::forward<Func>(func)), args_(std::forward<Args>(args)...) {}

    FutureWork(FutureWork&&) = default;

    FutureWork& operator=(FutureWork&&) = default;

    std::future<Ret> getFuture() {
        return task_.get_future();
    }

    void operator()() override {
        std::apply(task_, args_); // C++ 17 (https://en.cppreference.com/w/cpp/utility/apply) TODO: make it C++ 14
    }

private:
    std::packaged_task<Ret(Args...)> task_;
    std::tuple<Args...> args_;
};

class Instruction {
public:

    explicit Instruction(std::unique_ptr<Work> work) : work_(std::move(work)) {}

    Instruction(const Instruction&) = delete;

    Instruction(Instruction&&) = default;

    Instruction& operator=(const Instruction&) = delete;

    Instruction& operator=(Instruction&&) = default;

    bool isStop() const {
        return work_ == nullptr;
    }

    bool isWork() const {
        return work_ != nullptr;
    }

    Work& work() {
        return *work_;
    }

private:
    std::unique_ptr<Work> work_;
};

class InstructionQueue {
public:

    // TODO: factory for work() and stop()
    InstructionQueue() : work_queue_(), stop_count_(0), mtx_(), cond_() {}

    // std::size_t size() const {
    //     std::unique_lock<std::mutex> lock(mtx_);
    //     return work_queue_.size() + stop_count_;
    // }

    std::size_t stopCount() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return stop_count_;
    }

    std::size_t workCount() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return work_queue_.size();
    }

    void addStop(std::size_t count) {
        std::unique_lock<std::mutex> lock(mtx_);
        stop_count_ += count;
        lock.unlock();
        cond_.notify_all();
    }

    void addWork(std::unique_ptr<Work> work) {
        std::unique_lock<std::mutex> lock(mtx_);
        work_queue_.push(std::move(work));
        lock.unlock();
        cond_.notify_one();
    }

    Instruction waitInstruction() {
        std::unique_lock<std::mutex> lock(mtx_);

        cond_.wait(lock, [&] {
            return stop_count_ > 0 || !work_queue_.empty();
        });

        if (!work_queue_.empty()) {
            Instruction inst(std::move(work_queue_.front()));
            work_queue_.pop();
            return inst;
        } else {
            --stop_count_;
            return Instruction(nullptr);
        }
    }

private:
    std::queue<std::unique_ptr<Work>> work_queue_;
    std::size_t stop_count_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};


class Worker {
public:

    enum class State {
        STOPPED,
        WAITING,
        WORKING
    };

    explicit Worker(InstructionQueue& instruction_queue)
        : id_(id), state_(State::STOPPED), thr_(), instruction_queue_(instruction_queue) {}

    Worker(const Worker&) = delete;

    Worker(Worker&&) = delete;

    Worker& operator=(const Worker&) = delete;

    Worker& operator=(Worker&&) = delete;

    ~Worker() {
        std::unique_lock<std::mutex> lock(mtx_);

        if (thr_.joinable()) {
            lock.unlock();
            thr_.join();
        }
            
    }

    State state() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return state_;
    }

    void start() {
        std::unique_lock<std::mutex> lock(mtx_);
        if (state_ != State::STOPPED)
            throw "...";

        thr_ = std::thread(&Worker::loop, this);
    }

private:
    void loop() {
        std::unique_lock<std::mutex> lock(mtx_);

        log("starting");

        while (true) {
            state_ = State::WAITING;

            lock.unlock();
            Instruction inst = instruction_queue_.waitInstruction();
            lock.lock();

            if (inst.isStop()) {
                log("stopping");
                break;
            }

            log("working");

            state_ = State::WORKING;
            lock.unlock();
            inst.work()();
            lock.lock();
        }

        state_ = State::STOPPED;
    }

    State state_; // TODO: has data-race
    std::thread thr_;
    InstructionQueue& instruction_queue_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};


class Manager {
public:

    ~Manager() {
        instruction_queue_.addStop(workers_.size());
    }

    void hire(unsigned int count) {
        std::unique_lock<std::mutex> lock(mtx_);

        // TODO: STL filter?
        for (auto& worker : workers_) {
            if (worker->state() == Worker::State::STOPPED) {
                worker->start();
                count--;
            }
        }

        while (count--) {
            std::unique_ptr<Worker> worker = std::make_unique<Worker>();
            worker->start();
            workers_.push_back(std::move(worker));
        }
    }

    void fire(unsigned int count) {
        std::unique_lock<std::mutex> lock(mtx_);
        instruction_queue_.addStop(count);
    }

    void pushWork(std::unique_ptr<Work> work) {
        // std::unique_lock<std::mutex> lock(mtx_);
        instruction_queue_.addWork(std::move(work));
    }

    std::size_t workerCount() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return workers_.size();
    }

private:
    std::vector<std::unique_ptr<Work>> workers_;
    InstructionQueue instruction_queue_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

void hello() {
    std::cout << "hello world" << std::endl;
}

// void hello(std::string name, int age) {
//     std::cout << "hi " << name << ", your age is = " << age << std::endl;
// }

void some_void() {
    std::cout << "hi world" << std::endl;
}

int big_work() {
    for (long long int i = 0; i < 1e3L * 5L; i++);
    return 666;
}

// class Wrapper {
// public:
//     void set_work(Work& work) {
//         work_ = &work;
//     }

//     void do_work() {
//         work_->run();
//     }

// private:
//     Work *work_;
// };

int print_and_double(int x) {
    std::cout << x << std::endl;
    return x * 2;
}

int main() {
    constexpr int WORKER_COUNT = 4;

    Manager manager;

    // Worker worker(666, queue);
    // worker.start();

    // FutureWork<int> big(big_work);
    // std::future<int> f = big.getFuture();
    // queue.addWork(std::make_unique<FutureWork<int>>(std::move(big)));
    // queue.addStop(1);

    return 0;
}