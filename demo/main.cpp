#include <iostream>

#include "tpool/task.hpp"
#include "tpool/pool.hpp"
#include "tpool/work_queue.hpp"

/*
The command problem

Obvious way:
	* enum with type
	* (void *) with all the data (an object...)

CommandInterpreter:
	* a specific listener/handler for each command
	* onDoWork()
	* onShutdown()
*/

int main() {



	/*
	Worker
		* do work
		* has a inner thread
		* states (e.g. working, terminated, available)
		* how to shut it down?

	WorkQueue
		* store works
		* its shared between workers and "work suppliers"

	#1
		* Worker depends on WorkQueue
		* Waits until a work is available
		* But how do we terminate it?


	Worker
		do(Work)
		terminate()
		get_state()

		std::thread
		std::cond_var
		std::mutex



	/*
	2 possibilities

	1.
	WorkQueue
		* push work
		* wait for work to be pulled
		* on terminate, throw an exception to wait(), or something
		* all consumers will be terminated at the same time
		* no way to terminate a single Worker
	
	WorkConsumer / Worker
		* wait for work
		* do work
		* catch wait() termination exception

	2.
	WorkQueue
		* just a simple thread-safe queue of works
		* notify the cond_var when someone pushes a work?
	
	Worker
		* should share its cond_var with all other consumers
		* the cond_var is notified when a task is pushed
		* the cond_var is notified when a termination is requested
	
	How work-stealing will work in such cases?

	Idea:
	Work Stealing: also do some work on future.get() calls

	Join Worker and WorkQueue together?

	The problem:
		Each worker has a cond_var
		Any worker must be able to be terminated at any time
	*/

	/*
	can the same cond_var wait for both next() and shutdown()?
	if the worker has a inner queue, yes

	but how with a shared queue?

	only if we have some kind of command supplier

	the command supplier can give 2 kinds of commands:
		* do work (and give that work)
		* shutdown

	after one of these commands, a notify_one() may choose correctly which one should receive
	the "shutdown" or "do work" command

	but another problem is: how to know which worker received the "shutdown" command? (so we can
	remove it from the workers data structure)

	one way is a callback (onShutdown(), for example)
	and identify which worker is by its ID

	Work
		* status? (PENDING, COMPLETED)
		* callable that does the work
		* promise?
	
	Worker
		* inner thread
		* start() - it starts to wait and execute commands
		* onShutdown callback?
	
	workSupplier (TODO: better name)
		* has a list of workers
		* receives a work and passes to a Worker
		* has a function that remove one of its workers
	
	
	Since the communication will be based on commands, need to think on a good architecture for
	identify and run commands.


	*/


	/*
	tpool::FixedSizePool pool(1);

	std::future<int> f = pool.submit(sum, 1, 2);

	std::cout << f.get() << std::endl;
	*/

	/*
	tpool::WorkQueue work_queue;

	std::thread t1(consumer, std::ref(work_queue));
	std::thread t2(producer, std::ref(work_queue));

	t1.join();
	t2.join();
	*/

	return 0;
}
