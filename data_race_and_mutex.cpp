#include <iostream>
#include <mutex>
#include <string>
#include <exception>
#include <functional>
#include <fstream>
#include <thread>

std::mutex	mux;

void		shared_print(std::string message, int number);
void		print_funct(void);

/*
	More realistic example, where the mutex is a member of a class
	And is not a global variable
*/

class	LogFile
{
private:
	std::ofstream	f_stream;
	std::mutex		mu;
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
	void	shared_print(std::string message, int number)
	{
		std::lock_guard<std::mutex>	lock(mu);
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

	/*
		NEVER TO DO LIST

		1. never returns a refernce to the fstream object, it can give an ability to share this resource
		ommiting mutex

		2. never pass fstream object as a parameter to a function
	*/
};

void	write_to_log(LogFile &lf)
{
	for (int i = 0; i > -100; i--)
		lf.shared_print("sub_thread", i);
}

int main()
{
	/* std::thread	thread_1(print_funct);
	
	try
	{
		for (int i = 0; i < 100; i++)
			shared_print("main_thread: ", i);
		thread_1.join();
	}
	catch (...)
	{
		if (thread_1.joinable())
			thread_1.join();
	}*/ 

	LogFile	lf;
	std::thread	thread_2(write_to_log, std::ref(lf));
	
	for (int i = 0; i < 100; i++)
		lf.shared_print("main_thread", i);
	thread_2.join();
	lf.print_file();
	std::cin.get();
    return 0;
}

void		print_funct(void)
{
	for (int i = 0; i > -100; i--)
	{
		shared_print("thread_1", i);
	}
}

void		shared_print(std::string msg, int number)
{
	/*
		Using of the lock()/unlock() methods is not recommended
		In case of exception, the mutex can be locked forever
		and this resource, std::cout will be never accessed any more
	*/

	/*
		this is a risky code, that can cause problems

		mu.lock();
		if (number == 10)
		throw std::exception();
		std::cout << msg << ": " << number << std::endl;
		mu.unlock();
	*/

	/*
		RAII APPROACH
		this approach guarantees that after leaving this scope mutex will be unlocked
		when the lock_guard object will be destroyed, the mutex will be unlocked
	*/

	std::lock_guard<std::mutex>	guard(mux);
	if (number == 10)
		throw std::exception();
	std::cout << msg << ": " << number << std::endl;
}
