#include <thread>
#include <iostream>
#include <string>

void	call_me(const std::string &message)
{
	std::cout << message << std::endl;
}

/*
	std::thread(); - will create a thread, that does not represent any thread of execution
	thread.get_id() - will return a 0	
	threa.joinable() - will return FALSE, the thread can not be detach() or join() any more
*/

/*
	std::thread(func_ptr, ...) - construct a thread object that represents a new joinable thread of execution.
	thread.get_id() - will return some number that is not 0
	thread.joinable() - will return TRUE, the thread can be join() or detach()

	after calling join() or detached() once, the ID of the thread is 0, and it can not be join()/detach() any more
*/

int main()
{
	call_me("I am NOT called in the thread");
	std::thread thread_1(&call_me, "I am called in the thread\n");

	std::cout << "It is an ID of the thread (used not default constructor): ";
	std::cout << thread_1.get_id() << std::endl;

	std::thread thread_2;
	std::cout << "It is an ID o the thread (used default constructor): ";
	std::cout << thread_2.get_id() << std::endl;

	if (thread_1.joinable())
	{
		std::cout << "Yes, you can still join or detach me ..." << std::endl;
		
		// main thread waits for thread_1 to finish

		thread_1.join();
	}

	std::cout << "Press any key to exit ...." << std::endl;
	std::cin.get();

    return 0;
}

