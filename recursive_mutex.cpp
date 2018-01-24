#include <iostream>
#include <thread>
#include <mutex>

/*
	 the program will never terminates. The problem is very simple.
	 In the both() function, the thread acquires the lock and then calls the mul() function.
	 In this function, the threads tries to acquire the lock again, but the lock is already locked.
	 This is a case of deadlock. By default, a thread cannot acquire the same mutex twice.
*/

struct	Complex
{
	std::mutex	mu;
	int			nb;

	Complex(int n) : nb(n)
	{

	}

	void	mul(int n)
	{
		std::lock_guard<std::mutex>	locker(mu);
		nb *= n;
	}

	void	div(int n)
	{
		std::lock_guard<std::mutex>	locker(mu);
		nb /= n;
	}

	void	both(int n)
	{
		std::lock_guard<std::mutex>	locker(mu);
		mul(n);
		div(n);
	}
};

/*
	std::recursive_mutex. This mutex can be acquired several times by the same thread.
*/

struct	RecursiveComplex
{
	std::recursive_mutex	mu;
	int			nb;
	
	RecursiveComplex(int n) : nb(n)
	{

	}

	void	mul(int n)
	{
		std::lock_guard<std::recursive_mutex>	locker(mu);
		nb *= n;
	}

	void	div(int n)
	{
		std::lock_guard<std::recursive_mutex>	locker(mu);
		nb /= n;
	}

	void	both(int n1, int n2)
	{
		std::lock_guard<std::recursive_mutex>	locker(mu);
		mul(n1);
		div(n2);
	}
};

int main()
{
    return 0;
}

