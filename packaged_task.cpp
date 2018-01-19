#include <iostream>
#include <cmath>
#include <thread>
#include <future>
#include <mutex>
#include <deque>
#include <functional>

/*
	Ways to get a future:

	1. std::promise has a get_future();
	2. std::packaged_task has a get_future();
	3. std::async returns future<T>
*/

int		factorial(int n)
{
	int res;

	res = 1;
	for (int i = 1; i <= n; i++)
		res *= i;
	std::cout << "Result is: " << res << std::endl;
	return (res);
}

std::deque< std::packaged_task<int()> >	task_q;
std::mutex	mu;
std::condition_variable cond;

void	thread_func()
{
	std::packaged_task<int()>		task;
	{
		std::unique_lock<std::mutex>	locker(mu);
		cond.wait(locker, []() { return (!(task_q.empty())); });
		task = std::move(task_q.front());
		task_q.pop_front();
	}
	task();
}

int main()
{
	/*
		The class template std::packaged_task wraps any Callable target (function, lambda expression,
		bind expression, or another function object) so that it can be invoked asynchronously.
		Its return value or exception thrown is stored in a shared state which can be accessed
		through std::future objects.
	*/

	std::packaged_task<int(int)>	p1(factorial);

	p1(5);
	std::cout << p1.get_future().get() << std::endl;

	/*
		std::bind() creates a function object that can be invoked later
	*/

	std::packaged_task<int()>	p2(std::bind(factorial, 6));

	p2();

	auto bind_fact = std::bind(factorial, 10);
	bind_fact();

	std::packaged_task<int()>	package(std::bind(factorial, 5));
	std::future<int> fut_1 = package.get_future();
	std::thread	t1(thread_func);

	/*
		create a package that will be processed
		create a future that will give us a returned value from factorial function
		lock the mutex
		push back our task to the queue of the task
		wake up another thread
	*/
	{
		std::unique_lock<std::mutex>	locker(mu);
		task_q.push_back(package);
	}
	cond.notify_one();
    return (0);
}

