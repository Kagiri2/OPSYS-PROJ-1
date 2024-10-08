#include "Process.h"

Process::Process(std::string id, int arrive_time, bool cpu_bound, int context_switch_time)
    : pid(id), arrival_time(arrive_time), is_cpu_bound(cpu_bound),
      current_burst_index(0), io_completion_time(0),
      waiting_time(0), turnaround_time(0), response_time(-1),
      burst_estimate(0), remaining_time(0), tau(0), completed(false),
      t_cs(context_switch_time) {}

void Process::generate_bursts(int seed, int upper_bound, double lambda, bool is_cpu_bound) {
    int num_bursts = cpu.getCPUBurst();

    for (int j = 0; j < num_bursts; j++) {
        int cpu_time = cpu.getCPUBurstTime(seed, upper_bound, lambda, is_cpu_bound);
        int io_time = 0;
        if (j < num_bursts - 1) {
            io_time = cpu.getIOTime(seed, upper_bound, lambda, is_cpu_bound);
        }
        if(is_cpu_bound) {
            cpu_time += 4;
            io_time += 1;
        } else {
            cpu_time += 1;
            io_time += 8;
        }
        cpu_bursts.push_back(std::make_pair(cpu_time, io_time));
    }
}

int Process::get_next_cpu_burst() {
    if (current_burst_index < static_cast<int>(cpu_bursts.size())) {
        remaining_time = cpu_bursts[current_burst_index].first;
        return remaining_time;
    }
    return 0;
}

int Process::get_io_completion_time()
{
    return io_completion_time;
}

int Process::get_total_burst_time() const {
    if (current_burst_index < static_cast<int>(cpu_bursts.size())) {
        return cpu_bursts[current_burst_index].first;
    }
    return 0;
}

bool Process::is_completed() const {
    return completed;
}

void Process::update_completion_status() {
    completed = true;
}

int Process::start_io(int current_time) {
    if (current_burst_index < static_cast<int>(cpu_bursts.size()) - 1) {
        io_completion_time = current_time + cpu_bursts[current_burst_index].second + t_cs / 2;
        current_burst_index++;
        return io_completion_time;
    }
    return 0;
}

bool Process::is_io_completed(int current_time) {
    if(io_completion_time == 0) return false;

    if (current_time == io_completion_time) {
        return true;
    }
    return false;
}

void Process::reset() {
    current_burst_index = 0;
    io_completion_time = 0;
    waiting_time = 0;
    turnaround_time = 0;
    response_time = -1;
    remaining_time = 0;
    tau = 0;
    completed = false;
    preempted = false;
}

void Process::preempt(int time_used) {
    remaining_time = std::max(0, remaining_time - time_used);
    // if (remaining_time <= 0) {
    //     current_burst_index++;
    //     if (current_burst_index < cpu_bursts.size()) {
    //         remaining_time = cpu_bursts[current_burst_index].first;
    //     } else {
    //         remaining_time = 0;
    //     }
    // }
}

int Process::get_tau()
{
    return tau;
}

void Process::set_tau(int new_tau)
{
    tau = new_tau;
}