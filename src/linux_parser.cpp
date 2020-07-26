#include <unistd.h>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "linux_parser.h"

namespace fs = std::filesystem;

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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
    filestream.close();
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename, std::ifstream::in);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
  }
  return kernel;
}

std::string Stem(const fs::path &p) {
    return p.stem().string();
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for(const fs::directory_entry& d_entry : fs::directory_iterator(kProcDirectory)) {
    // Is this a directory?
    if (d_entry.is_directory()) {
      // Is every character of the name a digit?
      string filename(Stem(d_entry.path()));
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}

// TODO: Read and return the system memory utilization
double LinuxParser::MemoryUtilization() {
    string line;
    string key, val;
    double MemTotal = 0, MemFree = 0, Buffers = 0, Cached = 0, Slab = 0;
    
    std::ifstream filestream(kProcDirectory + kMeminfoFilename);
    if(filestream.is_open()) {
        while(std::getline(filestream, line)) {
            std::istringstream iss(line);
            iss >> key;
            if(key == "MemTotal:") {
                iss >> MemTotal;
            } else if(key == "MemFree:") {
                iss >> MemFree;
            } else if(key == "Buffers:") {
                iss >> Buffers;
            } else if(key == "Cached:") {
                iss >> Cached;
            } else if(key == "Slab:") {
                iss >> Slab;
            }
        }
        filestream.close();
    }
    double used = MemTotal - MemFree - Buffers - Cached - Slab;
    return used / MemTotal;
}

// TODO: Read and return the system uptime
double LinuxParser::UpTime() {
    string line;
    double retval = 0.0;
    std::ifstream filestream(kProcDirectory + kUptimeFilename);
    if(filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream iss(line);
        iss >> retval;
        filestream.close();
    }
    return retval;
}

// TODO: Read and return the number of jiffies for the system
unsigned long long LinuxParser::Jiffies() {
    double uptime = UpTime(); //in seconds
    return convertSecondsIntoClockTicks(uptime);
}

// TODO: Read and return the number of active jiffies for a PID
unsigned long long LinuxParser::ActiveJiffies(int pid) {
    unsigned long long utime { 0 }, stime { 0 }; //in clock ticks
    unsigned long long cutime { 0 },  cstime { 0 }; //in clock ticks
    string line;
    int posInLine = 14;
    std::string filePath { kProcDirectory + "/" + std::to_string(pid) + kStatFilename };
    std::ifstream filestream(filePath);
    if(filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream iss(line);
        int i = 0;
        string tmp;
        while(i < posInLine - 1) {
            iss >> tmp;
            i++;
        }
        iss >> utime >> stime >> cutime >> cstime;
        filestream.close();
    }
    return utime + stime + cutime + cstime;
}

// TODO: Read and return the number of active jiffies for the system
unsigned long long LinuxParser::ActiveJiffies() {
    vector<string> data = CpuUtilization();
    unsigned long long NonIdle = stoi(data[LinuxParser::CPUStates::kUser_])
                                 + stoi(data[LinuxParser::CPUStates::kNice_])
                                 + stoi(data[LinuxParser::CPUStates::kSystem_])
                                 + stoi(data[LinuxParser::CPUStates::kIRQ_])
                                 + stoi(data[LinuxParser::CPUStates::kSoftIRQ_])
                                 + stoi(data[LinuxParser::CPUStates::kSteal_]);
    return NonIdle;
}

// TODO: Read and return the number of idle jiffies for the system
unsigned long long LinuxParser::IdleJiffies() {
    vector<string> data = LinuxParser::CpuUtilization();
    unsigned long long Idle = stoi(data[LinuxParser::CPUStates::kIdle_]) 
                              + stoi(data[LinuxParser::CPUStates::kIOwait_]);
    return Idle;
}

// TODO: Read and return CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() {
    vector<string> data;
    string line;
    string key;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if(filestream.is_open()) {
        while(std::getline(filestream, line)) {
            std::istringstream iss(line);
            iss >> key;
            if (key == "cpu") {
                int i;
                string value;
                for(i = 0; i < 10; i++) {
                    iss >> value;
                    data.push_back(value);
                }
                break;
            }
        }
        filestream.close();
    }
    return data;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
    string line;
    string key;
    int retval = 0;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if(filestream.is_open()) {
        while(std::getline(filestream, line)) {
            std::istringstream iss(line);
            iss >> key;
            if (key == "processes") {
                iss >> retval;
            }
        }
        filestream.close();
    }
    return retval;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    string line;
    string key;
    int retval = 0;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if(filestream.is_open()) {
        while(std::getline(filestream, line)) {
            std::istringstream iss(line);
            iss >> key;
            if (key == "procs_running") {
                iss >> retval;
            }
        }
        filestream.close();
    }
    return retval;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
    string line;
    string cmd;
    std::string filePath { kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename };
    std::ifstream filestream(filePath);
    if(filestream.is_open()){
        std::getline(filestream, line);
        cmd = line;
        filestream.close();
    }
    return cmd;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
    string line;
    string key;
    string RAMInKB;
    std::string filePath { kProcDirectory + "/" + std::to_string(pid) + kStatusFilename };
    std::ifstream filestream(filePath);
    if(filestream.is_open()){
        while(std::getline(filestream, line)) {
            std::istringstream iss(line);
            iss >> key;
            if(key == "VmSize:") {
                iss >> RAMInKB;
                break;
            }
        }
        
        filestream.close();
    }
    return RAMInKB;
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
    string line;
    string key;
    string uid_val;
    std::string filePath { kProcDirectory + "/" + std::to_string(pid) + kStatusFilename };
    std::ifstream filestream(filePath);
    if(filestream.is_open()){
        while(std::getline(filestream, line)) {
            std::istringstream iss(line);
            iss >> key;
            if(key == "Uid:") {
                iss >> uid_val;
                break;
            }
        }
        
        filestream.close();
    }
    return uid_val;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
    string line;
    string curr_name, pwd, curr_uid;
    string username;
    string uid = Uid(pid);
    std::ifstream filestream(kPasswordPath);
    if(filestream.is_open()){
        while(std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream iss(line);
            iss >> curr_name >> pwd >> curr_uid;
            if(curr_uid == uid) {
                username = curr_name;
                break;
            }
        }
        filestream.close();
    }
    return username;
}

unsigned long long LinuxParser::StartTime(int pid) {
    string line;
    int posInLine = 22;
    unsigned long long starttime = 0ULL;
    std::string filePath { kProcDirectory + "/" + std::to_string(pid) + kStatFilename };
    std::ifstream filestream(filePath);
    if(filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream iss(line);
        int i = 0;
        string tmp;
        while(i < posInLine - 1) {
            iss >> tmp;
            i++;
        }
        iss >> starttime;
        filestream.close();
    }
    return starttime;
}

// TODO: Read and return the uptime of a process
double LinuxParser::UpTime(int pid) {
    double processStartTime = convertClockTicksIntoSeconds(StartTime(pid));
    double systemUpTime = UpTime();
    return systemUpTime - processStartTime;
}

double LinuxParser::convertClockTicksIntoSeconds(unsigned long long ticks) {
    long ticksPerSecond = sysconf(_SC_CLK_TCK);
    return ((double)ticks / ticksPerSecond);
}

unsigned long long LinuxParser::convertSecondsIntoClockTicks(double seconds) {
    long ticksPerSecond = sysconf(_SC_CLK_TCK);
    return (unsigned long long)(seconds * ticksPerSecond);
}