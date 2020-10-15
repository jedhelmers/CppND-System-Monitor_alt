#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{
	float utilization{0.0f};
	long total_system_jiffies{LinuxParser::Jiffies()};
	long active_system_jiffies{LinuxParser::ActiveJiffies()};

	long delta_active_jiffies = active_system_jiffies - prev_active_jiffies_;
	long delta_total_jiffies = total_system_jiffies - prev_total_jiffies_;
	if(delta_total_jiffies > 0)
	{
		utilization =  (1.0 * delta_active_jiffies)/delta_total_jiffies;
	}
	prev_active_jiffies_ = active_system_jiffies;
	prev_total_jiffies_ = total_system_jiffies;

	return utilization;
}