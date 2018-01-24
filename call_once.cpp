#include <thread>
#include <mutex>
#include <iostream>
#include <functional>

std::once_flag	flag;
std::mutex		mu;

void	print_message(void)
{
	std::call_once(flag, []() { std::lock_guard<std::mutex> locker(mu);
	std::cout << "I am called once ..." << std::endl; });

	std::lock_guard<std::mutex>	locker(mu);
	std::cout << "I am called each time ..." << std::endl;
}

int main()
{
	std::thread	t1(print_message);
	std::thread	t2(print_message);
	std::thread	t3(print_message);
	std::thread	t4(print_message);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	std::cin.get();
    return 0;
}

