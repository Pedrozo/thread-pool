#include <iostream>
#include <future>
#include <functional>

// template<typename Result, typename... Args>

class Task {
public:

	// template<typename Callable>
	// explicit Task(Callable&& work) : work_(std::forward<Callable>(work)) {}

	// explicit Task(std::function<Result(Args...)> work) : work_(std::move(work)), promise_() {}



	~Task() = default;

	Task(const Task&) = delete;

	Task(Task&&) = default;

	Task& operator=(const Task&) = delete;

	Task& operator=(Task&&) = default;

	std::future<Result> getFuture() {
		return promise_.get_future();
	}

	void operator()(Args&&... args) {
		promise_.set_value(work_(std::forward<Args>(args)...));
	}

private:
	// std::function<Result(Args...)> work_; // TODO: remove this!
	std::function<void()> work_;
	std::promise<Result> promise_;
};

int sum(int a, int b) {
	return a + b;
}

class Functor {
public:

	Functor() = default;

	Functor(const Functor&) = delete;

	Functor(Functor&&) = default;

	Functor& operator=(const Functor&) = delete;

	Functor& operator=(Functor&&) = default;

	double operator()(double a, double b) const {
		return a * b;
	}
};

int main() {
	Functor f;
	Task<double(double, double)> task(std::ref(f));

	std::thread t([&task] {
		std::future<double> f = task.getFuture();
		std::cout << "= " << f.get() << std::endl;
	});

	for (int i = 0; i < 1e9; i++);

	task(12.0, 13.0);

	t.join();

	/*
	Task<int(int,int)> task(sum);
	// Task<int, int, int> task(sum);

	std::thread t([&task] {
		std::future<int> f = task.getFuture();
		std::cout << "result = " << f.get() << std::endl;
	});

	for (int i = 0; i < 1e9; i++);

	task(66, 77);

	t.join();
	*/

	return 0;
}