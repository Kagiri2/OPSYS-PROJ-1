#include "process.h"

Process::Process(std::string id, int arrive_time, bool cpu_bound)
    : pid(id), arrival_time(arrive_time), is_cpu_bound(cpu_bound) {}

void Process::generate_bursts(int seed, int upper_bound, double lambda) {
    int num_bursts = cpu.getCPUBurst();

    for (int j = 0; j < num_bursts; j++) {
        int cpu_time = cpu.getCPUBurstTime(seed, upper_bound, lambda, is_cpu_bound);
        int io_time = 0;
        if (j < num_bursts - 1) {
            io_time = cpu.getIOTime(seed, upper_bound, lambda, is_cpu_bound);
        }
        cpu_bursts.push_back({cpu_time, io_time});
    }
}