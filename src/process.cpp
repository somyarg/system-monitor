#include <string>

#include "linux_parser.h"
#include "process.h"

using std::string;

// TODO: Return this process's ID
int Process::Pid() const { 
    return pid;
}

void Process::calculateCpuUsage() {
    double upTime = LinuxParser::UpTime(pid); // seconds
    unsigned long long activeTicks = LinuxParser::ActiveJiffies(pid);
    double activeTime = LinuxParser::convertClockTicksIntoSeconds(activeTicks); // total time spent for the process
    
    long double deltaUpTime = upTime - prevUpTime;
    long double deltaActiveTime = activeTime - prevActiveTime;

    updatePrevUpTime(upTime);
    updatePrevActiveTime(activeTime);

    long double cpuUsage = deltaActiveTime / deltaUpTime; //cpuUsage is a fraction in [0, 1]
    this->cpuUsage = cpuUsage;
}

// TODO: Return this process's CPU utilization
long double Process::CpuUtilization() {
    return cpuUsage;
}

// TODO: Return the command that generated this process
string Process::Command() {
    if (command.empty()) {
        command = LinuxParser::Command(pid);
    }
    return command;
}

// TODO: Return this process's memory utilization
string Process::Ram() {
    string RAMInKB = LinuxParser::Ram(pid);
    double RAMInMB = convertKBIntoMB(std::stod(RAMInKB));
    char buffer[32] { 0 };
    sprintf(buffer, "%.2f", RAMInMB);
    return string(buffer);
}

// TODO: Return the user (name) that generated this process
string Process::User() {
    if (user.empty()) {
        user = LinuxParser::User(pid);
    }
    return user;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
    return (long int)(LinuxParser::UpTime(pid)); // seconds
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return this->cpuUsage > a.cpuUsage;
}

void Process::updatePrevUpTime(double upTime) {
    prevUpTime = upTime;
}

void Process::updatePrevActiveTime(double activeTime) {
    prevActiveTime = activeTime;
}

double Process::convertKBIntoMB(double valInKB) {
    return valInKB / 1000.0;
}