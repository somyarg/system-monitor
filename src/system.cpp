#include <string>
#include <vector>
#include <algorithm>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
    processes_.clear();
    vector<int> pids = LinuxParser::Pids();
    for (const auto &pid : pids) {
        Process x { pid };
        x.calculateCpuUsage();
        processes_.push_back(x);
    }
    sort(processes_.begin(), processes_.end()); // Sort in descending order of CPUUtilisation
    return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {
    if(kernel.empty()) {
        kernel = LinuxParser::Kernel();
    }
    return kernel;
}

// TODO: Return the system's memory utilization
double System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

// TODO: Return the operating system name
std::string System::OperatingSystem() {
    if(OS.empty()) {
        OS = LinuxParser::OperatingSystem();
    }
    return OS;
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() {
    return (long int)(LinuxParser::UpTime());
}