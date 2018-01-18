#include <deque>
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>
#include <condition_variable>

std::mutex				mu;
std::condition_variable cond;
std::deque<int>			q;

void	function_1(void)
{
	int i;

	i = 10;
	while (i > 0)
	{
		std::unique_lock<std::mutex>	locker(mu);
		q.push_front(i);
		locker.unlock();
		
		/*
			Notify one waiting thread, if there is one
			wakes up another thread, it is sleeping
		*/

		cond.notify_one();								
		std::this_thread::sleep_for(std::chrono::seconds(1));
		i--;
	}
}

void	function_2(void)
{
	int data;

	data = 0;
	while (data != 1)
	{
		std::unique_lock<std::mutex>	locker(mu);

		/*
			wait() takes as a parameter a mutex and a predicate;
			if predicate returns FALSE the thread continues to sleep
		*/

		cond.wait(locker, []() { return (!q.empty()); });
		data = q.back();
		q.pop_back();
		locker.unlock();
		std::cout << "t2 got value from t1: " << data << std::endl;
	}
}

int main()
{
	std::thread t1(function_1);
	std::thread t2(function_2);

	t1.join();
	t2.join();
    return (0);
}

