#include <atomic>
#include <thread>
#include <iostream>
#include <vector>

struct AtomicCounter
{
	std::atomic<int>	counter;

	AtomicCounter()
	{
		counter = 0;
	}

	void	increment(void)
	{
		++counter;
	}
	void	decrement(void)
	{
		counter--;
	}
};

struct Counter
{
	int	counter;

	Counter()
	{
		counter = 0;
	}

	void	increment(void)
	{
		++counter;
	}
	void	decrement(void)
	{
		counter--;
	}
};

int main()
{
	std::vector<std::thread>	non_atom;
	std::vector<std::thread>	atom;
	Counter						count;
	AtomicCounter				a_count;

	std::cout << "Count from 0 to 100 NON ATOMIC COUNTER in 5 threads" << std::endl;

	for (int i = 0; i < 5; i++)
	{
		non_atom.push_back(std::thread([&count] { 
			for (int j = 0; j < 100; j++)
				count.increment();
		}));
	}
	for (int i = 0; i < 5; i++)
		non_atom[i].join();
	std::cout << "RESULT: " << count.counter << std::endl;
	std::cout << "Count from 0 to 500 ATOMIC COUNTER in 5 threads" << std::endl;
	for (int i = 0; i < 5; i++)
	{
		atom.push_back(std::thread([&a_count] {
			for (int j = 0; j < 100; j++)
				a_count.increment();
		}));
	}
	for (int i = 0; i < 5; i++)
		atom[i].join();
	std::cout << "RESULT: " << a_count.counter << std::endl;
	std::cin.get();
    return (0);
}

