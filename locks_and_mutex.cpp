#include <thread>
#include <chrono>
#include <mutex>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <set>
#include <iostream>
#include <cstdarg>

/*
	The lock() and unlock() methods should be straight forward.
	The first locks the mutex, blocking if the mutex is not available, and the later unlocks the mutex.
*/

std::mutex	mu;
void		thread_function(void);

template <typename T>
class container
{
	/*
		std::mutex	mu can cause a deadlock, but in my case It is an memory access problem
		The reason is the container attempts to acquire the mutex multiple times before releasing it
		and that is not possible. That is where std::recursive_mutex come into the picture.
		It allows a thread to acquire the same mutex multiple times.

		The constructors of these wrapper guards have overloads that take an argument
		indicating the locking strategy. The available strategies are:

		* defer_lock of type defer_lock_t: do not acquire ownership of the mutex
		* try_to_lock of type try_to_lock_t: try to acquire ownership of the mutex without blocking
		* adopt_lock of type adopt_lock_t: assume the calling thread already has ownership of the mutex
	*/

	mutable std::recursive_mutex	mut;
	std::vector<T>					data;
public:
	void add(T element)
	{
		std::lock_guard<std::recursive_mutex>	locker(mut);
		
		// mut.lock();
		
		data.push_back(element);
		
		// mut.unlock();
	}
	void add_range(int num, ...)
	{
		va_list arguments;
		va_start(arguments, num);
		for (int i = 0; i < num; i++)
		{
			std::lock_guard<std::recursive_mutex>	locker(mut);
			
			// mut.lock();
			
			add(va_arg(arguments, T));
			
			// mut.unlock();
		}
		va_end(arguments);
	}
	void dump() const
	{
		/*
			method dump() should be constant, beacause it does not change the state of the object
			but the usage std::lock_guard<> uses RAII approach, that changes object
			that is why  void dump() const will cause a compilation error

			A mutex (regardless which implementation is used) must be acquired and release
			and that implies calls to the non-constant lock() and unlock() methods.

			In order to make dump() method const we make mutable std::recursive_mutex mu, it will
			ignore that the method is constant
		*/

		std::lock_guard<std::recursive_mutex>	locker(mut);
		
		// mut.lock();
		
		for (auto e : data)
			std::cout << e << std::endl;
		
		// mut.unlock();
	}
};

void		thread_function_list(container<int> &c)
{
	std::srand((unsigned int)std::time(NULL));
	c.add_range(3, rand(), rand(), rand());

}

/*
	new tamplate container to hold a data of type T
*/

template<typename T>	
class set_container
{
public:
	std::mutex	mu;
	std::set<T>	data;

	void		add(T elem)
	{
		data.insert(elem);
	}
	void		remove(T elem)
	{
		data.erase(elem);
	}
};

/*
	example of unsafe exchange() function that will cause deadlock

	Suppose this function is called from two different threads, from the first, an element is removed from container 1 and added to container 2
	and in the second it is removed from container 2 and added to container 1.
	This can lead to a deadblock (if the thread context switches from one thread to another
	just after acquiring the first lock).
*/

void	exchange_unsafe(set_container<int> &c1, set_container<int> &c2, int val)
{
	c1.mu.lock();
	std::this_thread::sleep_for(std::chrono::seconds(1)); // simulating a deadlock
	c2.mu.lock();
	c1.remove(val);
	c1.add(val);
	c1.mu.unlock();
	c2.mu.unlock();
}

void exchange(set_container<int> &c1, set_container<int> &c2, int value)
{
	std::lock(c1.mu, c2.mu);
	c1.remove(value);
	c2.add(value);
	c1.mu.unlock();
	c2.mu.unlock();
}

int main()
{

	std::thread	thread_1(thread_function);
	std::thread thread_2(thread_function);
	std::thread thread_3(thread_function);

	thread_1.join();
	thread_2.join();
	thread_3.join();

	container<int>	cntr;
	std::thread t1(thread_function_list, std::ref(cntr));
	std::thread t2(thread_function_list, std::ref(cntr));
	std::thread t3(thread_function_list, std::ref(cntr));

	t1.join();
	t2.join();
	t3.join();

	cntr.dump();

	srand((unsigned int)time(NULL));
	set_container<int> cntr1;
	cntr1.add(1);
	cntr1.add(2);
	cntr1.add(3);
	set_container<int> cntr2;
	cntr2.add(4);
	cntr2.add(5);
	cntr2.add(6);
	std::thread tt1(exchange, std::ref(cntr1), std::ref(cntr2), 3);
	std::thread tt2(exchange, std::ref(cntr2), std::ref(cntr1), 6);
	tt1.join();
	tt2.join();

	std::cin.get();

    return 0;
}

void	thread_function(void)
{
	mu.lock();

	int time;

	std::cout << "Entered a thread: " << std::this_thread::get_id() << std::endl;
	time = 1 + std::rand() % 10;
	std::cout << "I will fall asleep for a " << time << " seconds ..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(time));
	std::cout << "Leaving a thread: " << std::this_thread::get_id() << std::endl;

	mu.unlock();
}

