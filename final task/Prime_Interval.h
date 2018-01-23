#pragma once

#include <iostream>
#include <exception>
#include <thread>
#include <mutex>
#include <regex>
#include <vector>
#include <utility>
#include <fstream>
#include <string>

class Prime_Interval
{
private:
	std::string											fname;
	std::string											data;
	std::ofstream										output;
	std::ifstream										input;
	std::mutex											write_mutex;
	std::mutex											read_mutex;
	std::vector<int>									primes;
	std::vector<std::exception_ptr>						error_list;
	std::vector< std::pair<int, int> >					intervals;
	std::vector< std::string >							open_tag;
	std::vector< std::string >							close_tag;
	std::vector< std::pair<std::string, std::string> >	tag_args;
	std::string											reg_expr;

	void								make_regex(void);
	int									count_words(const std::string &str1, const std::string &str2);
	void								remove_string_part_last(const std::string &tag_1,
											const std::string &tag_2);
	void								remove_string_part_first(const std::string &tag_1,
											const std::string &tag_2);
	int									is_numeric(const std::string &str);
	int									is_prime(int nb);
	std::string							take_digit();
public:
										Prime_Interval() = delete;
										~Prime_Interval();
										Prime_Interval(const std::string &file_name);
	void								open_file_read(void);
	void								print_data(void) const;
	void								validate_parse_file(void);
	void								print_errors(void) const;
	void								process_root_tag(void);
	void								process_intervals_tag(void);
	void								process_interv_tag(void);
	void								process_low_high_tag(void);
	void								print_intervals(void) const;
	void								print_to_console(void) const;
	int									threads_number(void) const;
	void								make_unique(void);
	void								process_interval(int index);
	void								write_to_file(void);
	std::thread							count_prime_numbers(int index);

	/*
		Classes of exceptions that will handle some specific errors
		that may occurr during the file processing
	*/

	class	File_exception : public std::exception
	{
	public:
		const char *what(void) const throw() override
		{
			return ("ERROR: NO SUCH FILE OR IT CAN'T BE OPENNED.");
		}
	};
	class	File_structure_exception : public std::exception
	{
		const char *what() const throw() override
		{
			return ("ERROR: INVALID STRUCTURE OF A FILE.");
		}
	};
	class	Not_a_number_exception : public std::exception
	{
		const char *what(void) const throw() override
		{
			return ("ERROR: not a number.");
		}
	};
	class	Low_greater_High_exception : public std::exception
	{
		const char *what(void) const throw() override
		{
			return ("ERROR: lower limit is greater than high.");
		}
	};
};

