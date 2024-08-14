#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
#include "CPU.h"

class Process {
public:
    Process(std::string id, int arrive_time, bool cpu_bound);

    void generate_bursts(int seed, int upper_bound, double lambda, bool is_cpu_bound);

    // Getters
    std::string get_pid() const { return pid; }
    int get_arrival_time() const { return arrival_time; }
    const std::vector<std::pair<int, int> >& get_cpu_bursts() const { return cpu_bursts; }
    int get_current_burst_index() const { return current_burst_index; }
    bool is_cpu_bound_process() const { return is_cpu_bound; }
    int get_num_bursts() const { return cpu_bursts.size(); }
    int get_next_cpu_burst();
    int get_io_completion_time();
    bool is_completed() const;
    int start_io(int current_time);
    bool is_io_completed(int current_time);
    void reset();
    void set_completed(bool status) { completed = status; }
    void update_completion_status();
    void set_waiting_time(int time) { waiting_time = time; }
    int get_waiting_time() const { return waiting_time; }
    void set_turnaround_time(int time) { turnaround_time = time; }
    int get_turnaround_time() const { return turnaround_time; }
    void set_response_time(int time) { response_time = time; }
    int get_response_time() const { return response_time; }
    void update_burst_estimate(double new_estimate) { burst_estimate = new_estimate; }
    double get_burst_estimate() const { return burst_estimate; }
    int get_remaining_time() const { return remaining_time; }
    void preempt(int time_used);
    int get_tau();
    void set_tau(int new_tau);
    int get_total_burst_time() const;
    void increment_io_completion_time() { io_completion_time++;}
    void set_io_completion_time(int time) { io_completion_time = time; }

private:
    std::string pid;
    int arrival_time;
    std::vector<std::pair<int, int> > cpu_bursts;
    bool is_cpu_bound;
    CPU cpu;
    int current_burst_index;
    int io_completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    double burst_estimate;
    int remaining_time;
    int tau;
    bool completed;
};

#endif