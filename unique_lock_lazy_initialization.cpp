#include <iostream>
#include <mutex>
#include <string>
#include <exception>
#include <functional>
#include <fstream>
#include <thread>

class	LogFile
{
private:
	std::ofstream	f_stream;
	std::mutex		mu;
	std::once_flag	open_flag;
public:
	LogFile()
	{
	}
	~LogFile()
	{
		if (f_stream.is_open())
			f_stream.close();
	}
	void	shared_print(std::string message, int number)
	{
		/*
			std::unique_lock allows to create an object withot locking it during construction
			it can be lock() unlock() as many times as needed
			it can not be copeid, only moved
			And if you need a condition variables, std::unique_lock should be used
			std::defer_lock creates an object that is not locked
		*/

		/*
			initialiazation upon first use idiom / lazy nisitialization
			once_flag makes sure that the file will be opened only one time
		*/
		if (!f_stream.is_open())
		{
			std::call_once(open_flag, [&](){ f_stream.open("log.txt"); });
		}
		std::unique_lock<std::mutex> locker(mu, std::defer_lock);
		locker.lock();
		f_stream << message << ": " << number << std::endl;
		locker.unlock();
	}
	void	print_file(void)
	{
		std::ifstream if_str;
		std::string temp;

		if_str.open("log.txt");
		while (std::getline(if_str, temp))
			std::cout << temp << std::endl;
		if_str.close();
	}
};

void	write_to_log(LogFile &lf)
{
	for (int i = 0; i > -100; i--)
		lf.shared_print("sub_thread", i);
}

int main()
{

	LogFile	lf;
	std::thread	thread_1(write_to_log, std::ref(lf));

	for (int i = 0; i < 100; i++)
		lf.shared_print("main_thread", i);
	thread_1.join();
	lf.print_file();
	std::cin.get();
	return 0;
}