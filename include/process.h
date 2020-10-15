#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
 Process(int pid);
  int Pid() const;
  std::string User();
  std::string Command();
  float CpuUtilization() const;
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a);

 
 private:
 int pid_{-1};
 float process_active_seconds_prev_{0.0f};
 float time_elapsed_process_start_prev_{0.0f};
};

#endif