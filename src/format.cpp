#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) 
{
	int seconds_remainder = seconds%60;
	int minutes = seconds / 60;
	int minutes_remainder = minutes%60;
	int hours = seconds/3600;

	string elapsed_time{""};
	if(hours < 10)
	{
		elapsed_time += "0";
	}
	elapsed_time += std::to_string(hours);
	elapsed_time += ":";
	if(minutes_remainder < 10)
	{
		elapsed_time += "0";
	}
	elapsed_time += std::to_string(minutes_remainder);
	elapsed_time += ":";
	if(seconds_remainder < 10)
	{
		elapsed_time += "0";
	}
	elapsed_time += std::to_string(seconds_remainder);

	return elapsed_time;
}