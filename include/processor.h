#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"
class Processor {
 public:
  float Utilization();

 private:
 	long prev_active_jiffies_{0};
 	long prev_total_jiffies_{0};
};

#endif