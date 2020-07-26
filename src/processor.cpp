#include <vector>
#include <string>

#include "linux_parser.h"
#include "processor.h"

using std::string;
using std::vector;
using std::stoi;

// TODO: Return the aggregate CPU utilization
double Processor::Utilization() {
    vector<string> data = LinuxParser::CpuUtilization();
    unsigned long long NonIdle = stoi(data[LinuxParser::CPUStates::kUser_])
                                 + stoi(data[LinuxParser::CPUStates::kNice_])
                                 + stoi(data[LinuxParser::CPUStates::kSystem_])
                                 + stoi(data[LinuxParser::CPUStates::kIRQ_])
                                 + stoi(data[LinuxParser::CPUStates::kSoftIRQ_])
                                 + stoi(data[LinuxParser::CPUStates::kSteal_]);
    unsigned long long Idle = stoi(data[LinuxParser::CPUStates::kIdle_]) 
                              + stoi(data[LinuxParser::CPUStates::kIOwait_]);
    unsigned long long deltaNonIdle = NonIdle - PrevNonIdle;
    unsigned long long deltaIdle = Idle - PrevIdle;
    double retval = ((double)deltaNonIdle) / (deltaNonIdle + deltaIdle);
    return retval;
}

void Processor::updatePrevIdle(unsigned long long Idle) {
    PrevIdle = Idle;
}

void Processor::updatePrevNonIdle(unsigned long long NonIdle) {
    PrevNonIdle = NonIdle;
}