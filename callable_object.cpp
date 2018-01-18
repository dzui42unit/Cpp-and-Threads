#include <thread>
#include <mutex>
#include <iostream>

/*
	Simple functor with no purpose for demonstartion
*/

class	Functor
{
public:
	Functor()
	{

	}
	~Functor()
	{

	}
	void	foo(int n, char ch)
	{
		std::cout << "Foo function called: " << n << ", " << ch << std::endl;
	}
	long	bar(double n)
	{
		std::cout << "Bar function called: " << n << std::endl;
		return (0);
	}
	int		operator()(int n)
	{
		std::cout << "operator() called: " << n << std::endl;
		return (0);
	}
};

void	func(void)
{
	
}

int main()
{
	Functor	functor;

	std::thread thread_1(functor, 1);						// called a copy_of_functor() in a different thread
	std::thread thread_2(std::ref(functor), 2);				// called functor() in a different thread
	std::thread thread_3(Functor(), 3);						// called temp Functor()
	std::thread thread_4([](int x) { return (x * x); }, 4); // calling a lambda function in the thread
	std::thread thread_5(func);								// calling a user function in a thread
	std::thread thread_6(&Functor::foo, functor, 6, 'a');	// calling copy_of_functor.foo() in a thread
	std::thread thread_7(&Functor::foo, &functor, 6, 'a');	// calling of the functor.foo() in a thread
	std::thread thread_8(std::move(functor), 8);			// functor is no longer available

	thread_1.join();
	thread_2.join();
	thread_3.join();
	thread_4.join();
	thread_5.join();
	thread_6.join();
	thread_7.join();
	thread_8.join();

	std::cin.get();
    return (0);
}

