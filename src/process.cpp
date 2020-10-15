#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
{
	if(pid > 0)
	{
		pid_ = pid;
	}
}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const
{
	int pid = Pid();
	float cpu_usage{0};
	if(pid > 0)
	{
		long process_active_jiffies{LinuxParser::ActiveJiffies(pid)};
		float process_active_seconds = process_active_jiffies/sysconf(_SC_CLK_TCK);

		long system_uptime_seconds{LinuxParser::UpTime()};

		long process_uptime_jiffies{LinuxParser::UpTime(pid)};
		long process_uptime_seconds = process_uptime_jiffies/sysconf(_SC_CLK_TCK);

		float time_elapsed_process_start = system_uptime_seconds - process_uptime_seconds;

		float delta_process_active_seconds = process_active_seconds - process_active_seconds_prev_;
		float delta_time_elapsed_process_start = time_elapsed_process_start - time_elapsed_process_start_prev_;

		if(delta_time_elapsed_process_start != 0)
		{
			cpu_usage = 1.0*((delta_process_active_seconds)/(delta_time_elapsed_process_start));
		}

	}

	return cpu_usage;
}

string Process::Command() 
{
	string command{""};
	int pid = Pid();
	if(pid > 0)
	{
		command = LinuxParser::Command(pid);
	}

	return command;
}

string Process::Ram() 
{
	string ram{""};
	int pid = Pid();
	if(pid > 0)
	{
		ram = LinuxParser::Ram(pid);
	}

	return ram;
}

string Process::User()
{
	string user{""};
	int pid = Pid();
	if(pid > 0)
	{
		user = LinuxParser::User(pid);
	}

	return user;
}

long int Process::UpTime() {
	int pid = Pid();
	long int uptime{0};
	if(pid > 0)
	{
		uptime = LinuxParser::UpTime(pid);
	}

	return uptime/sysconf(_SC_CLK_TCK);
}

bool Process::operator<(Process const& a) 
{
	return (CpuUtilization() < a.CpuUtilization());
}