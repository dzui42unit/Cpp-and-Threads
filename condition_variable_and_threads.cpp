#include <queue>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <random>
#include <condition_variable>

bool					done;
bool					notify;
std::mutex				print_mutex;
std::mutex				queue_mutex;
std::queue<int>			error_log;
std::condition_variable	cond;

void		worker_function(int id, std::mt19937 &generator)
{
	/*
		prints the id of the worker and says that it works
	*/

	{
		std::unique_lock<std::mutex>	locker(print_mutex);
		std::cout << "[worker" << id << "]\trunning ...." << std::endl;
	}

	/*
		simulating of the work, making some random delay
	*/

	std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));

	/*
		simulating of an error occurrance
	*/

	{
		std::unique_lock<std::mutex>	locker(print_mutex);
		std::cout << "[worker " << id << "]\tan arror occured ..." << std::endl;
	}

	/*
		reporting an error and adding it to the queue
	*/
	int err_code = (1 + generator() % 5) * 100 + 1;
	{
		std::unique_lock<std::mutex>	locker(queue_mutex);
		error_log.push(err_code);
		notify = true;
		cond.notify_one();
	}
}

void	logger_function(void)
{
	/*
		starting message
	*/

	{
		std::unique_lock<std::mutex>	locker(print_mutex);
		std::cout << "[logger]\trunning ...." << std::endl;
	}

	/*
		while the signal will be passed
	*/

	while (!done)
	{
		std::unique_lock<std::mutex>	locker(queue_mutex);

		/*
			to avoid spurious wakeup
		*/

		while (!notify)
			cond.wait(locker);
		
		/*
			used another overloaded version of the cond.wait(locker, predicate);
			cond.wait(locker, [&]() { return !(error_log.empty()); });

		*/

		while (!error_log.empty())
		{
			std::unique_lock<std::mutex>	qlock(print_mutex);
			std::cout << "[logger]\tprocessing error: " << error_log.front() << std::endl;
			error_log.pop();
		}
		notify = false;
	}
}

std::mutex				lock;
std::condition_variable cond_var;

void		worker_function2(std::mt19937 &generator)
{
	/*
		prints the id of the worker and says that it works
	*/

	{
		std::unique_lock<std::mutex>	locker(print_mutex);
		std::cout << "[worker]\trunning ...." << std::endl;
	}

	/*
		simulating of the work, making some random delay
	*/

	std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));

	{
		std::unique_lock<std::mutex>	locker(print_mutex);
		std::cout << "[worker]\tfinished ..." << std::endl;
	}
	std::unique_lock<std::mutex>	ulock(lock);
	done = true;
	std::notify_all_at_thread_exit(cond_var, std::move(ulock));
}

int main()
{
	/*
		initialising a random number generator
	*/

	std::mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

	/*
		running of a logger
	*/

	std::thread	log_thread(logger_function);

	/*
		running of workers
	*/

	std::vector<std::thread>	workers;
	for (int i = 0; i < 5; i++)
		workers.push_back(std::thread(worker_function, i + 1, std::ref(generator)));
	for (auto &e : workers)
		e.join();

	done = true;
	log_thread.join();

	std::cout << "main running..." << std::endl;
	std::thread worker(worker_function2, std::ref(generator));
	worker.detach();
	std::cout << "main crunching..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));
	{
		std::unique_lock<std::mutex> locker(print_mutex);
		std::cout << "main waiting for worker..." << std::endl;
	}
	std::unique_lock<std::mutex> lock(lock);
	while (!done)
		cond_var.wait(lock);
	std::cout << "main finished..." << std::endl;

	std::cin.get();
    return (0);
}

