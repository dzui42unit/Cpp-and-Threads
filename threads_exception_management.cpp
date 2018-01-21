#include <thread>
#include <mutex>
#include <string>
#include <exception>
#include <vector>
#include <iostream>

std::mutex							mu;
std::vector<std::exception_ptr>		except_storage;

void	theead_exception_safe_function(void);
void	function(int &a);
void	test_exception(int a);

/*
	thead_1.join() - block a calling thread (main thread in our case)
	main thead waits untill the thead_1 will finish its work

	thead_1.detach() - will separates the thread object from its execution
	allows it to execute independently
*/

/*
	<thread> header provides some helper functions in namespace std::this_thread:

	* get_id: returns the id of the current thread
	* yield: tells the scheduler to run other threads and can be used when you are in a busy waiting state
	* sleep_for: blocks the execution of the current thread for at least the specified period
	* sleep_util: blocks the execution of the current thread until the specified moment of time has been reached
*/

int main()
{
	/*
		Creation of a thread and passing var by reference uning wrapper-class std::ref();
		Joining it, after this a thread in not joinable any more
	*/

	int var;

	var = 1;
	std::thread	thread_1(function, std::ref(var));
	thread_1.join();

	/*
		This code is unsafe, the exception that was thrown by a test_exception() function will not
		be catched in this case.

		try
		{
			std::thread thread_2(test_exception, 22);
			std::thread thread_3(test_exception, 12);

			thread_2.join();
			thread_3.join();
		}
		catch (std::exception &ex)
		{
			std::cout << ex.what() << std::endl;
		}

	*/

	/*
		Better way is to catch exception in the function that works in the thread
		for further processing in other parts of the program, in some storage

		std::vector<std::exception_ptr>	exc_storage;
	*/

	except_storage.clear();
	std::thread	thread_exc1(theead_exception_safe_function);
	std::thread thread_exc2(theead_exception_safe_function);

	thread_exc1.join();
	thread_exc2.join();

	for (auto &e : except_storage)
	{
		try
		{
			if (e != nullptr)
				std::rethrow_exception(e);
		}
		catch (const std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}

	std::cin.get();

    return 0;
}

void	function(int &a)
{
	std::cout << "I am in a thead ..." << std::endl;
	a++;
}

void	test_exception(int a)
{
	std::string message;

	message = "Something went wrong with " + std::to_string(a) + " number ....";
	if (a == 12)
		throw (std::exception(message.c_str()));
}

void	theead_exception_safe_function(void)
{
	try
	{
		test_exception(22);
		test_exception(12);
	}
	catch (...)
	{
		/*
			captures the current exception object and creates an std::exception_ptr
			that holds either a copy or a reference to that exception object
		*/

		std::lock_guard<std::mutex>	locker(mu);
		except_storage.push_back(std::current_exception());
	}
}