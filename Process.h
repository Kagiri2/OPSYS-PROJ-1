#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
#include "cpu.h"

class Process {
public:
    Process(std::string id, int arrive_time, bool cpu_bound);

    void generate_bursts(int seed, int upper_bound, double lambda);

    std::string get_pid() const { return pid; }
    int get_arrival_time() const { return arrival_time; }
    const std::vector<std::pair<int, int>>& get_cpu_bursts() const { return cpu_bursts; }
    bool is_cpu_bound_process() const { return is_cpu_bound; }
    int get_num_bursts() const { return cpu_bursts.size(); }

private:
    std::string pid;
    int arrival_time;
    std::vector<std::pair<int, int>> cpu_bursts;
    bool is_cpu_bound;
    CPU cpu;
};

#endif
