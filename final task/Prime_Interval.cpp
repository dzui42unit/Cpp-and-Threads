#include "stdafx.h"
#include "Prime_Interval.h"

Prime_Interval::~Prime_Interval()
{

}

Prime_Interval::Prime_Interval(const std::string & file_name)
{
	fname = file_name;
	primes.clear();
	data.clear();
	error_list.clear();
	intervals.clear();
}

void	Prime_Interval::write_to_file(void)
{
	output.open("result.txt");
	output << "<root>\n\t";
	output << "<primes>";
	for (int i = 0; i < primes.size(); i++)
	{
		output << " " << primes[i];
	}
	output << " </primes>\n";
	output << "</root>";
	output.close();
}

int		Prime_Interval::is_prime(int nb)
{
	int start;
	
	if (nb == 0 || nb == 1)
		return (1);
	start = 2;
	while (start < nb)
	{
		if ((nb % start) == 0)
			return (0);
		start++;
	}
	return (1);
}

void Prime_Interval::print_to_console(void) const
{
	for (int i = 0; i < primes.size(); i++)
		std::cout << "Element #" << i + 1 << " = " << primes[i] << std::endl;
}

int		Prime_Interval::threads_number(void) const
{
	return ((int)intervals.size());
}

void		Prime_Interval::process_interval(int index)
{
	int low;
	int high;

	{
		std::lock_guard<std::mutex>	locker(read_mutex);
		low = intervals[index].first;
		high = intervals[index].second;
	}
	while (low <= high)
	{
		if (is_prime(low))
		{
			std::lock_guard<std::mutex>	locker(write_mutex);
			primes.push_back(low);
		}
		low++;
	}
}

std::thread	Prime_Interval::count_prime_numbers(int index)
{
	return (std::thread([=] { process_interval(index); }));
}

void	Prime_Interval::make_unique(void)
{
	std::sort(primes.begin(), primes.end());
	auto last = std::unique(primes.begin(), primes.end());
	primes.erase(last, primes.end());
}

void	Prime_Interval::open_file_read(void)
{
	std::string		temp;

	input.open(fname);
	try
	{
		if (!input.is_open())
			throw (Prime_Interval::File_exception());
	}
	catch (Prime_Interval::File_exception &e)
	{
		std::cout << e.what() << std::endl;
		exit(0);
	}
	while (std::getline(input, temp))
	{
		temp.erase(std::remove_if(temp.begin(), temp.end(), [](char ch) { return (isspace(ch)); }), temp.end());
		data += temp;
		temp.clear();
	}
	input.close();
}

void	Prime_Interval::validate_parse_file(void)
{
	try
	{
		process_root_tag();
		for (int i = 0; i < intervals.size(); i++)
		{
			if (intervals[i].first > intervals[i].second)
				throw(Prime_Interval::Low_greater_High_exception());
		}
	}
	catch (...)
	{
		error_list.push_back(std::current_exception());
	}
}

void	Prime_Interval::process_root_tag(void)
{
	/*
		check the presence of the <root> ... </root> tag and removing it from a data string
		if it is not present or closed openned in a different manner, throw an exception
	*/

	std::regex	root_tag("<root>.*</root>");

	if (!std::regex_match(data, root_tag))
		throw(Prime_Interval::File_structure_exception());
	remove_string_part_last("<root>", "</root>");
	process_intervals_tag();
}

void	Prime_Interval::process_intervals_tag(void)
{
	/*
		check the presence of the <intervals> ... </intervals> tag and removing it from a data string
		if it is not present or closed openned in a different manner, throw an exception
	*/

	std::regex	root_tag("<intervals>.*</intervals>");

	if (!std::regex_match(data, root_tag))
		throw(Prime_Interval::File_structure_exception());
	remove_string_part_last("<intervals>", "</intervals>");
	process_interv_tag();
}

void		Prime_Interval::make_regex(void)
{

	/*
		method that each time fulfills regural expression that is used for validating a file
		on each step of parsing / validating a new tag is added
	*/

	reg_expr.clear();
	reg_expr += (open_tag[0] + tag_args[0].first);
	for (int i = 1; i < open_tag.size(); i++)
		reg_expr += (open_tag[i] + tag_args[i].first + close_tag[i] + tag_args[i].second);
	reg_expr += (close_tag[0] + tag_args[0].second);
}

void		Prime_Interval::process_interv_tag(void)
{
	/*
		check the presence of the <interval> ... </interval> tag and removing it from a data string
		if it is not present or closed openned in a different manner, throw an exception

		<interval> ... </interval> tag can be present as many times as needed
		that is why we are searching for it untill we will not clear our string
	*/

	while (!data.empty())
	{
		/*
			all data is creared for the creation of a regular expression fron the very beginning
		*/

		close_tag.clear();
		open_tag.clear();
		tag_args.clear();
		open_tag.push_back("<interval>");
		close_tag.insert(close_tag.begin(), "</interval>");
		tag_args.push_back(std::pair<std::string, std::string>("(.*)", "(.*)?"));
		make_regex();
		if (!std::regex_match(data, std::regex(reg_expr.c_str())))
			throw(Prime_Interval::File_structure_exception());
		process_low_high_tag();
	}
}

void		Prime_Interval::process_low_high_tag(void)
{
	/*
		checking <low></low> and <high></high> tags and taking data from them
		And pushing it the container of data for intervals
		Changing regural expression for doing different tests
	*/

	std::pair<int, int>	temp;

	tag_args.front().first = "";
	open_tag.push_back("<low>");
	close_tag.push_back("</low>");
	tag_args.push_back(std::pair<std::string, std::string>("(.*)", "(.*)"));
	make_regex();
	if (!std::regex_match(data, std::regex(reg_expr)))
		throw(Prime_Interval::File_structure_exception());
	tag_args.back().first = "[0-9]+";
	make_regex();
	if (!std::regex_match(data, std::regex(reg_expr)))
		throw(Prime_Interval::Not_a_number_exception());
	open_tag.push_back("<high>");
	close_tag.push_back("</high>");
	tag_args.push_back(std::pair<std::string, std::string>("(.*)", ""));
	make_regex();
	if (!std::regex_match(data, std::regex(reg_expr)))
		throw(Prime_Interval::File_structure_exception());
	tag_args.back().first = "[0-9]+";
	make_regex();
	if (!std::regex_match(data, std::regex(reg_expr)))
		throw(Prime_Interval::Not_a_number_exception());
	remove_string_part_first("<interval>", "</interval>");
	remove_string_part_first("<low>", "</low>");
	temp.first = std::stoi(take_digit());
	remove_string_part_first("<high>", "</high>");
	temp.second = std::stoi(take_digit());
	intervals.push_back(temp);
}

std::string	Prime_Interval::take_digit(void)
{
	std::string number;
	int			start;
	int			end;

	start = 0;
	while (!isdigit(data[start]))
		start++;
	end = start;
	while (isdigit(data[end]))
	{
		number.push_back(data[end]);
		end++;
	}
	data.erase(data.begin() + start, data.begin() + end);
	return (number);
}

void		Prime_Interval::print_intervals(void) const
{
	for (auto e : intervals)
	{
		std::cout << "START: " << e.first << " END: " << e.second << std::endl;
	}
}

int		Prime_Interval::is_numeric(const std::string &str)
{
	for (int i = 0; i < str.length(); i++)
	{
		if (!isdigit(str[i]))
			return (0);
	}
	return (1);
}

void	Prime_Interval::print_data(void) const
{
	std::cout << data << std::endl << std::endl;
}

void	Prime_Interval::print_errors(void) const
{
	for (auto &e : error_list)
	{
		try
		{
			if (e != nullptr)
			{
				std::rethrow_exception(e);
			}
		}
		catch(const std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}

void		Prime_Interval::remove_string_part_first(const std::string &tag_1, const std::string &tag_2)
{
	/*
		removes two parst of the string: first from the beginning, second part is the first
		occurance of the tag_2 string
	*/

	size_t		start_pos;
	size_t		end_pos;
	
	start_pos = data.find(tag_1);
	data.erase(data.begin() + start_pos, data.begin() + tag_1.length() + start_pos);
	end_pos = data.find(tag_2);
	data.erase(data.begin() + end_pos, data.begin() + end_pos + tag_2.length());
}

void		Prime_Interval::remove_string_part_last(const std::string &tag_1, const std::string &tag_2)
{
	/*
		removes a parts of string: first from the beginning, second from the end
	*/

	size_t		end_pos;

	data.erase(data.begin(), data.begin() + std::string(tag_1).length());
	end_pos = data.rfind(tag_2);
	data.erase(data.begin() + end_pos, data.end());
}

int		Prime_Interval::count_words(const std::string &open, const std::string &close)
{
	int		count;
	size_t	pos;

	pos = 0;
	count = 0;
	while ((pos = data.find(open, pos)) != std::string::npos)
	{
		count++;
		pos += open.length();
	}
	pos = 0;
	while ((pos = data.find(close, pos)) != std::string::npos)
	{
		pos += open.length();
		count++;
	}
	return (count);
}
