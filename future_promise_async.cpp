#include <future>
#include <thread>
#include <iostream>

int		factorial(int n)
{
	int res;
	
	res = 1;
	for (int i = 1; i <= n; i++)
		res *= i;
	return (res);

}	

int		factorial2(std::future<int> &f)
{
	int res;
	int	n;

	res = 1;

	/*
		If the promise will be broken, throws an exception
	*/

	n = f.get();
	for (int i = 1; i <= n; i++)
		res *= i;
	return (res);
}

int main()
{
	std::future<int>	thread_result;

	/*
		std::future<T> allows us to get a result from the asynchronous function call
		this one code will call a factorial function in a new/same thread
		To get a result from a child thread to the parent thread
	*/

	thread_result = std::async(factorial, 5);
	std::cout << thread_result.get() << std::endl;
	
	/*
		future.get() can be called once, or it will crash a program	
	*/

	std::future<int>	res;

	/*
		This time the factorial function will be called in the same thread
		and will not be called untill the res.get() function will be called
	*/

	res = std::async(std::launch::deferred, factorial, 4);
	std::cout << res.get() << std::endl;

	/*
		This time the factorial function will be called in the another thread
		std::launch::async
	*/

	res = std::async(std::launch::async, factorial, 15);
	std::cout << res.get() << std::endl;

	/*
		std::promise - we can pass a value to a child thread from a parent in thre future
		std::promise can not be copied, only moved
		I can send only one promise to each thread, because they call get() method
		get() method can be called obly once
	*/

	std::promise<int>	prom;
	std::future<int>	fut_val;

	std::shared_future<int> sh_fut = fut_val.share();

	/*
		Creates a future that can be copied and passed to multiple threads
	*/

	fut_val = prom.get_future();
	res = std::async(std::launch::async, factorial2, std::ref(fut_val));
	
	/*
		If the value can not be set
		prom.set_exception(std::make_exception_ptr(std::runtime_error("Human error")));
	*/

	prom.set_value(6);
	std::cout << res.get() << std::endl;
	std::cin.get();
    return 0;
}

