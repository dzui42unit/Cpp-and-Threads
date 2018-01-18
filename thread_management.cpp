#include <iostream>
#include <string>
#include <thread>
#include <exception>
#include <functional>

void	call_me(std::string source, std::string &msg)
{
	std::cout << source << " says: " << msg << std::endl;
	msg = "Voice of slavery";
}

/*
	If the object of the thread will be destroyed before jon()/detach(), the program will be terminated

	std::thread	*new_thread;

	new_thread = new std::thread(call_me);
	delete new_thread;

	this code will cause an TERMINATING of the program
*/

class	Fctor
{
public:
	void	operator()(std::string msg)
	{
		std::cout << "thred_1 says: " << msg << " " << std::endl;
	}
};

int main()
{
	/* 
		std::thread	thread_1(call_me);
		First example with a simple function */
	/*
		Create an object of a functor that we will pass to the thread
	*/

	/*
		First way of creating a thread and passing it a functor;
		Create an object to a functor;
		Pass it to the constructor;

		Fctor		fct;
		std::thread	thread_1(fct);

		Second way is to use such syntax

		If we will not write ((Fctor()) the expression will be trated as a function declaration;
		std::thread	thread_1((Fctor());
	*/

	/*
		Oversubscpription - using more threads or resources than the hardware can allow to do this
		std::thread::hardware_concurency() allows to check the number of threads we can split into
	*/

	std::cout << "Indication: " << std::thread::hardware_concurrency() << std::endl;

	std::string message("Voice of freedom");

	/*
		std::this_thread namespace allows us to call the methods of the parent thread
	*/

	std::cout << "ID of the parent thread: " << std::this_thread::get_id() << std::endl;

	std::thread	thread_1((Fctor()), message);

	/*
		the parameters to the thread are always passed by value
	*/

	try
	{
		/*
			We are doing some code. It can cause an exception and we need to be sure
			that the thread will be join()/detach() before the deleting of an object
		*/

		for (int i = 0; i < 100; i++)
		{
			/* 
				if (i == 10)
					throw (std::exception());
			*/
		}
		std::cout << "main thread says: " << message << " " << std::endl;
	}
	catch (...)
	{
		/*
			In case of exception the program will not terminate, because the thread_1 will be join();
			If not, TERMINATING will occurr
		*/

		std::cout << "I have caught an exception." << std::endl;
		thread_1.join();
		std::cin.get();
		exit(1);
	}
	if (thread_1.joinable())
		thread_1.join();

	/*
		Trying to pass a string by reference and changing its value inside of function
		As a result the string has the same value, it was passed by value
	*/

	std::thread	thread_2(call_me, "thread_2", message);

	thread_2.join();
	call_me("main thead", message);

	/*
		If we want to pass something to the thread by a reference
		we need to use a std::ref() it a wrapper-class to hold a reference
	*/

	/*
		Also we can use std::move() funstion;
		std::move(message);
		but in this case the value of message will be empty
	*/

	std::thread thread_3(call_me, "thead_3", std::ref(message));
	thread_3.join();
	call_me("main thead", message);

	std::cin.get();
	return (0);
}

