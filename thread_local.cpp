#include <mutex>
#include <string>
#include <thread>
#include <functional>
#include <iostream>

thread_local int	thr_var;
std::mutex			mu;

/*
	A thread_local object comes into existence when a thread starts and is destroyed when the thread ends.
	Each thread has its own instance of a thread-Local object.
*/

void	print_number(void)
{
	std::lock_guard<std::mutex>	locker(mu);
	std::cout << "Value thr_val in a thread " << std::this_thread::get_id() << " is " << thr_var << std::endl;
}

void	work_in_thread(int nb)
{
	thr_var = nb;
	print_number();
}

int main()
{
	std::cout << "Maximum number of threads available: " << std::thread::hardware_concurrency() << std::endl;
	thr_var = 1;
	std::thread t1(work_in_thread, 5);
	std::thread t2(work_in_thread, 20);
	t1.join();
	t2.join();
	std::cout << "Main thread value: " << thr_var << std::endl;
	std::cin.get();
	return (0);
}

