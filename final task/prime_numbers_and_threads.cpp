#include "Prime_Interval.h"

int main()
{
	std::string					file_name;
	Prime_Interval				*interval;
	std::vector<std::thread>	threads;

	std::cout << "Enter a name of file to process: ";
	std::getline(std::cin, file_name);
	interval = new Prime_Interval(file_name);
	interval->open_file_read();
	interval->validate_parse_file();
	interval->print_errors();
	for (int i = 0; i < interval->threads_number(); i++)
		threads.push_back(interval->count_prime_numbers(i));
	for (int i = 0; i < interval->threads_number(); i++)
		threads[i].join();
	interval->make_unique();
	interval->print_to_console();
	interval->write_to_file();
	std::cin.get();
	delete (interval);
    return (0);
}

