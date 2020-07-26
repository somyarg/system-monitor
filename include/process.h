#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(const int &pid) : pid(pid) {}
  
  int Pid() const;                         // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  long double CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  void calculateCpuUsage();

  // TODO: Declare any necessary private members
 private:
  void updatePrevUpTime(double upTime);
  void updatePrevActiveTime(double activeTime);
  double convertKBIntoMB(double valInKB);

  int pid { -1 };
  std::string command;
  std::string user;
  
  double prevUpTime { 0 }; //seconds
  double prevActiveTime { 0 }; //seconds
  long double cpuUsage { 0 }; //fraction between 0 and 1
};

#endif