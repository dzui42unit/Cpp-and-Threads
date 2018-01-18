#include <iostream>
#include <mutex>
#include <string>
#include <exception>
#include <functional>
#include <fstream>
#include <thread>

/*
	Tips to avoid deadlocks.

	1. Prefer a single mutex locking.
	2. Avoid locking a mutex and then calling a user function.
	3. Use std::lock() to lock more than one mutex.
	4. Lock mutexes in the same order.
*/

class	LogFile
{
private:
	std::ofstream	f_stream;
	std::mutex		mu1;
	std::mutex		mu2;
public:
	LogFile()
	{
		f_stream.open("log.txt");
	}
	~LogFile()
	{
		if (f_stream.is_open())
			f_stream.close();
	}
	void	shared_print1(std::string message, int number)
	{
		/*
			This code causes a deadlock
			The problem is that the mutexes are locked in different order
			Here mu1, then mu2

			std::lock_guard<std::mutex>	lock1(mu1);
			std::lock_guard<std::mutex> lock2(mu2);
		*/

		/*
			First sulution is to lock mutexes in the same order;

			std::lock_guard<std::mutex>	lock1(mu1);
			std::lock_guard<std::mutex> lock2(mu2);
		*/

		/*
			Second solution is to use std::lock() function
			It allows to lock mutexes without deadlock
			In the constructor of lock_guard we need  to pass std::adopt_lock, it assumes that mutexes were
			alredy locked.
		*/

		std::lock(mu1, mu2);
		std::lock_guard<std::mutex> lock1(mu1, std::adopt_lock);
		std::lock_guard<std::mutex> lock2(mu2, std::adopt_lock);
		f_stream << message << ": " << number << std::endl;
	}
	void	shared_print2(std::string message, int number)
	{
		/*
			This code causes a deadlock
			The problem is that the mutexes are locked in different order
			Here mu2, then mu1

			std::lock_guard<std::mutex>	lock2(mu2);
			std::lock_guard<std::mutex> lock1(mu1);
		*/

		/*
			First sulution is to lock mutexes in the same order;

			std::lock_guard<std::mutex>	lock1(mu1);
			std::lock_guard<std::mutex> lock2(mu2);
		*/

		/*
			Second solution is to use std::lock() function
			It allows to lock mutexes without deadlock
			In the constructor of lock_guard we need  to pass std::adopt_lock, it assumes that mutexes were
			alredy locked.
		*/
		
		std::lock(mu1, mu2);
		std::lock_guard<std::mutex> lock2(mu2, std::adopt_lock);
		std::lock_guard<std::mutex> lock1(mu1, std::adopt_lock);
		f_stream << message << ": " << number << std::endl;
	}
	void	print_file(void)
	{
		std::ifstream if_str;
		std::string temp;

		if_str.open("log.txt");
		while (std::getline(if_str, temp))
			std::cout << temp << std::endl;
	}
};

void	write_to_log(LogFile &lf)
{
	for (int i = 0; i > -100; i--)
		lf.shared_print1("sub_thread", i);
}

int main()
{

	LogFile	lf;
	std::thread	thread_1(write_to_log, std::ref(lf));

	for (int i = 0; i < 100; i++)
		lf.shared_print2("main_thread", i);
	thread_1.join();
	lf.print_file();
	std::cin.get();
	return 0;
}