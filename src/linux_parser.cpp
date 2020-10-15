#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() 
{
  string line;
  string key, value;
  //creating variables for Total memory and Free Memory
  float MemTotal, MemFree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == kMemTotal){MemTotal = stof(value);}
      else if (key == kMemFree){MemFree = stof(value);}
    }
  }
    return (MemTotal - MemFree)/ MemTotal;
}

long LinuxParser::UpTime() 
{
  long uptime{0};

  string line;
  string uptime_string;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime_string;
    uptime = stof(uptime_string);
  }

  return uptime;
}

long LinuxParser::Jiffies() 
{
  return (ActiveJiffies() + IdleJiffies());
}

long LinuxParser::ActiveJiffies(int pid) 
{
  //active jiffies = #14_utime + #15_stime + #16_cutime + #17_cstime
  long active_jiffies{0};
  string line;
  string jiff;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    
    for(int i=1; i<=17; i++)
    {
      linestream >> jiff;
      if(i >=14 && i<=17)
      {
        active_jiffies += stol(jiff);
      }
    }
  }

  return active_jiffies;
}

long LinuxParser::ActiveJiffies() 
{
  //cpu user nice system idle iowait irq softirq steal guest guest_nice
  //active = user + nice + system + irq + softirq + steal

  long active_jiffies{0};
  vector<CPUStates> active_states = {
    kUser_,
    kNice_,
    kSystem_,
    kIRQ_,
    kSoftIRQ_,
    kSteal_,
    kGuest_,
    kGuestNice_
  };

  vector<string> cpu_utilization = CpuUtilization();
  for (auto state: active_states) {
    active_jiffies += stol(cpu_utilization[state]);
  }

  return active_jiffies;

}

long LinuxParser::IdleJiffies()
{
  //cpu user nice system idle iowait irq softirq steal guest guest_nice
  //idle = idle + iowait

  long idle_jiffies{0};
  vector<CPUStates> idle_states = {
    kIdle_,
    kIOwait_,
  };

  vector<string> cpu_utilization = CpuUtilization();
  for (auto state: idle_states) {
    idle_jiffies += stol(cpu_utilization[state]);
  }

  return idle_jiffies;

}

vector<string> LinuxParser::CpuUtilization()
{
  vector<string> cpu_utilization;

  //cpu user nice system idle iowait irq softirq steal guest guest_nice
  string cpu, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  string line;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
  }

  cpu_utilization.push_back(user);
  cpu_utilization.push_back(nice);
  cpu_utilization.push_back(system);
  cpu_utilization.push_back(idle);
  cpu_utilization.push_back(iowait);
  cpu_utilization.push_back(irq);
  cpu_utilization.push_back(softirq);
  cpu_utilization.push_back(steal);
  cpu_utilization.push_back(guest);
  cpu_utilization.push_back(guest_nice);

  return cpu_utilization;

}

int LinuxParser::TotalProcesses() 
{
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open())
  {
    while(std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == kTotalProcesses)
      {
        return stoi(value);
      }
    }
  }

  return 0;
}

int LinuxParser::RunningProcesses() 
{
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open())
  {
    while(std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == kProcessesRunning)
      {
        return stoi(value);
      }
    }
  }

  return 0;
}

string LinuxParser::Command(int pid) 
{
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(filestream.is_open())
  {
    std::getline(filestream, line);
  }

  return line;
}

string LinuxParser::Ram(int pid) 
{
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(filestream.is_open())
  {
    while(std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == kProcessMemoryUsed)
      {
        int ram_mb = stol(value) * 0.001;
        return to_string(ram_mb);
      }
    }
  }
  return string();
}

string LinuxParser::Uid(int pid) 
{
  string line;
  string key, value;
  string uid;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(filestream.is_open())
  {
    while(std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == kProcessUid)
      {
        uid = value;
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid) 
{
  string req_uid = Uid(pid);
  if(req_uid != "")
  {
    string line;
    string user_name, x, uid;

    std::ifstream filestream(kPasswordPath);
    if(filestream.is_open())
    {
      while(std::getline(filestream, line))
      {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> user_name >> x >> uid;
        if(uid == req_uid)
        {
          return user_name;
        }
      }
    }
  }
  return string();
}

long int LinuxParser::UpTime(int pid)
{
  //active jiffies = #22_start time
  long start_time{0};
  string line;
  string jiff;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int i=1; i<=22; i++)
    {
      linestream >> jiff;
      if(i==22)
      {
        start_time = stol(jiff);
      }
    }
  }

  return start_time;
}