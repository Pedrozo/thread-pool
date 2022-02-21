#include <iostream>
#include <functional>
#include <future>
#include <thread>
#include <queue>
#include <memory>

class Work {
public:
	virtual ~Work() = default;
	virtual void doWork() = 0;
};

template<typename Ret, typename... Args>
class FutureWork : public Work {
public:

	template<typename Func>
	FutureWork(Func&& func, Args&&... args) : task_(std::forward<Func>(func)), args_(std::forward<Args>(args)...) {}

	std::future<Ret> getFuture() {
		return task_.get_future();
	}

	void doWork() override {
		std::apply(task_, args_); // C++ 17 (https://en.cppreference.com/w/cpp/utility/apply) TODO: make it C++ 14
	}

private:
	std::packaged_task<Ret(Args...)> task_;
	std::tuple<Args...> args_;
};


/*
class Task {
public:

private:
	struct Callable {
		virtual ~Callable() = default;
		virtual void operator()() = 0;
	};

	template<typename Ret, typename... Args>
	struct WrappedTask : public Callable {

		template<typename Func>
		WrappedTask(Func&& func, Args&&... args)
			: task_(std::forward<Func>(func)), args_(std::forward<Args>(args)...) {}

		void operator()() override {
			std::apply(task_, args_);
		}

		std::packaged_task<Ret(Args...)> task_;
		std::tuple<Args...> args_;
	};

	std::unique_ptr<Callable> work_;
};
*/

int sum(int a, int b) {
	return a + b;
}

int sub(int a, int b) {
	return a - b;
}

int main() {
	std::vector<std::unique_ptr<Callable>> works;

	std::unique_ptr<Holder<int, int, int>> p1 = std::make_unique<Holder<int, int, int>>(sum, 1, 2);
	std::unique_ptr<Holder<int, int, int>> p2 = std::make_unique<Holder<int, int, int>>(sub, 1, 2);

	std::future<int> f1 = p1->getFuture();
	std::future<int> f2 = p2->getFuture();

	works.push_back(std::move(p1));
	works.push_back(std::move(p2));

	for (const auto& c : works)
		(*c)();

	std::cout << f1.get() << std::endl;
	std::cout << f2.get() << std::endl;

	/*
	works.push(std::make_unique<Holder<int, int, int>>(sum, 1, 2));
	works.push(std::make_unique<Holder<int, int, int>>(sub, 1, 2));
	*/

	/*
	std::unique_ptr<Holder<int, int, int>> holder = std::make_unique<Holder<int, int, int>>(sum, 1, 2);

	std::future<int> f = holder->getFuture();

	(*holder)();

	std::cout << f.get() << std::endl;
	*/

	return 0;
}