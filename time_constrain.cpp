#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>

int		factorial(int n)
{
	int res;

	res = 1;
	for (int i = 1; i <= n; i++)
		res *= i;
	std::cout << "Result is: " << res << std::endl;
	return (res);
}

int main()
{
	/*
		Threads
	*/

	std::thread t1(factorial, 6);
	std::this_thread::sleep_for(std::chrono::seconds(3));
	std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now() + std::chrono::seconds(5);
	std::this_thread::sleep_until(tp);
	t1.join();

	/*
		Mutex
	*/

	std::mutex	mu;

	std::lock_guard<std::mutex>		locker1(mu);
	std::unique_lock<std::mutex>	ulocker(mu);
	
	ulocker.try_lock();
	ulocker.try_lock_for(std::chrono::seconds(10));
	ulocker.try_lock_until(tp);

	/*
		Condition variable
	*/

	std::condition_variable cond;

	cond.wait_for(ulocker, std::chrono::milliseconds(10));
	cond.wait_until(ulocker, tp);

	/*
		Future and Promise
	*/
	
	std::promise<int>	prom;
	std::future<int>	fut;

	fut = prom.get_future();
	fut.get();
	fut.wait();
	fut.wait_for(std::chrono::seconds(2));
	fut.wait_until(tp);

	std::cin.get();
    return 0;
}

