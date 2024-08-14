#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cmath> 
#include "Process.h"
#include "next_exp.h"
#include "Totaller.h"
#include <queue>
#include <list>
#include <sstream>


void simulate_fcfs(std::vector<Process>& processes, int t_cs);
void simulate_sjf(std::vector<Process>& processes, int t_cs, double alpha, double lambda);
void simulate_srt(std::vector<Process>& processes, int t_cs, double alpha, double lambda);
void simulate_rr(std::vector<Process>& processes, int t_cs, int t_slice);
double estimate_next_burst(double previous_estimate, int actual_burst, double alpha);

// HELPER FUNCTIONS FOR SIMULATION
bool all_processes_completed(const std::vector<Process>& processes);
void reset_processes(std::vector<Process>& processes);
int get_next_process_sjf(const std::vector<Process*>& ready_queue, double alpha);
int get_next_process_srt(const std::vector<Process*>& ready_queue, Process* current_process, double alpha);
void print_simulation_results(const std::string& algorithm, const std::vector<Process>& processes, int t_cs);

// PART 2 OUTPUT FUNCTIONS
void print_verbose_output(const std::string& event, int time, const Process& process);
void print_algorithm_start(const std::string& algorithm);
void print_algorithm_end(const std::string& algorithm);

void calculate_statistics(const std::vector<Process>& processes, int t_cs);
void print_statistics(const std::string& algorithm);

double ceilhelper(double value) {
    return std::ceil(value * 1000.0) / 1000.0;
}

std::vector<Process> generate_processes(int n, int ncpu, int seed, double lambda, int upper_bound) {
    std::vector<Process> processes;
    CPU cpu;

    for (int i = 0; i < n; i++) {
        std::string pid = std::string(1, 'A' + i / 10) + std::to_string(i % 10);
        bool is_cpu_bound = i < ncpu;
        int arrival_time = cpu.getArrivalTime(seed, upper_bound, lambda);

        Process p(pid, arrival_time, is_cpu_bound);
        p.generate_bursts(seed, upper_bound, lambda, is_cpu_bound);
        processes.push_back(p);
    }

    return processes;
}

void print_processes(const std::vector<Process>& processes, Totaller& tot) {
    for (std::vector<Process>::const_iterator process = processes.begin(); process != processes.end(); ++process) {
        bool is_cpu_bound = process->is_cpu_bound_process();
        if (is_cpu_bound) {
            std::cout << "CPU-bound process ";
        } else {
            std::cout << "I/O-bound process ";
        }
        std::cout << process->get_pid() << ": arrival time " << process->get_arrival_time() << "ms; "
                  << process->get_num_bursts();
        
        if (process->get_num_bursts() == 1) {
            std::cout << " CPU burst:" << std::endl;
        } else {
            std::cout << " CPU bursts:" << std::endl;
        }

        /* NOT NEEDED FOR PART 2
        const std::vector<std::pair<int, int>>& cpu_bursts = process->get_cpu_bursts();
        for (std::vector<std::pair<int, int>>::const_iterator burst = cpu_bursts.begin(); burst != cpu_bursts.end(); ++burst) {
            int cpu_burst_time = burst->first;
            int io_burst_time = burst->second;

            if (is_cpu_bound) {
                cpu_burst_time += 4;
                tot.inc_ccbt(cpu_burst_time);
                tot.inc_ccbtn();
                std::cout << "==> CPU burst " << cpu_burst_time << "ms";
            } else {
                cpu_burst_time += 1;
                tot.inc_icbt(cpu_burst_time);
                tot.inc_icbtn();
                std::cout << "==> CPU burst " << cpu_burst_time << "ms";
            }

            if (burst != cpu_bursts.end() - 1) {  // Not the last burst
                if (is_cpu_bound) {
                    io_burst_time += 1;
                    tot.inc_cibt(io_burst_time);
                    tot.inc_cibtn();
                    std::cout << " ==> I/O burst " << io_burst_time << "ms";
                } else {
                    io_burst_time += 8;
                    tot.inc_iibt(io_burst_time);
                    tot.inc_iibtn();
                    std::cout << " ==> I/O burst " << io_burst_time << "ms";
                }
            }
            std::cout << std::endl;
        }
        */
    }
}

    std::string print_queue(const std::vector<Process*>& q) {
        if (q.empty()) return "empty";
        std::string result;
        for (const Process* p : q) {
            result += p->get_pid() + " ";
        }
        return result.substr(0, result.length() - 1);  // Remove trailing space
    }

    std::string print_queue(const std::list<Process*>& q) {
        if (q.empty()) return "empty";
        std::string result;
        for (const Process* p : q) {
            result += p->get_pid() + " ";
        }
        return result.substr(0, result.length() - 1);  // Remove trailing space
    }

    std::string print_queue(const std::queue<Process*>& ready_queue) {
        std::ostringstream oss;
        std::queue<Process*> temp_queue = ready_queue;

        while (!temp_queue.empty()) {
            oss << temp_queue.front()->get_pid();
            temp_queue.pop();
            if (!temp_queue.empty()) {
                oss << " ";
            }
        }

        return oss.str().empty() ? "empty" : oss.str();
    }



    // PART 2 FUNCTIONS
    void simulate_fcfs(std::vector<Process>& processes, int t_cs) {
        int current_time = 0;
        std::vector<Process*> ready_queue;
        Process* current_process = nullptr;
        int context_switch_remaining = 0;
        bool switching_out = false;

        std::cout << "time 0ms: Simulator started for FCFS [Q empty]" << std::endl;

        while (true) {

            // Check for new arrivals
            for (Process& p : processes) {
                if (p.get_arrival_time() == current_time) {
                    //std::cout << "ARRIVAL time " << current_time << std::endl;
                    ready_queue.push_back(&p);
                    if(current_time < 10000) {
                        std::cout << "time " << current_time << "ms: Process " << p.get_pid() 
                            << " arrived; added to ready queue [Q " << print_queue(ready_queue) << "]" << std::endl;
                    }
                }
            }

            // Check for I/O completion
            for (Process& p : processes) {
                if (p.is_io_completed(current_time)) {
                    //std::cout << "IO time " << current_time << std::endl;
                    ready_queue.push_back(&p);

                    if(current_time < 10000) {
                        std::cout << "time " << current_time << "ms: Process " << p.get_pid() 
                            << " completed I/O; added to ready queue [Q " << print_queue(ready_queue) << "]" << std::endl;
                    }
                }
            }

            // Handle context switch
            if (context_switch_remaining > 0) {
                context_switch_remaining--;
                if (context_switch_remaining == 0) {
                    // if we are done switching OUT a process (aka CPU is idle)
                    if (switching_out) {
                        switching_out = false;
                        current_process = nullptr;
                    } else if (current_process != nullptr) {
                        // the CPU isn't idle and we start using it
                        int burst_time = current_process->get_next_cpu_burst();
                        if(current_time < 10000) {
                            std::cout << "time " << current_time << "ms: Process " << current_process->get_pid() 
                                << " started using the CPU for " << burst_time << "ms burst [Q " 
                                << print_queue(ready_queue) << "]" << std::endl;
                        }
                    }
                }
            }

            // Start new process if CPU is idle
            if (current_process == nullptr && !ready_queue.empty() && context_switch_remaining == 0) {
                current_process = ready_queue.front();
                ready_queue.erase(ready_queue.begin());
                context_switch_remaining = t_cs / 2;
            }

            // Process execution
            if (current_process != nullptr && context_switch_remaining == 0) {
                current_process->preempt(1); // decreases the burst time by 1
                if (current_process->get_remaining_time() == 0) {
                    int remaining_bursts = current_process->get_num_bursts() - current_process->get_current_burst_index() - 1;
                    if (remaining_bursts == 0) {
                        std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " terminated [Q " << print_queue(ready_queue) << "]" << std::endl;
                        switching_out = true;
                        context_switch_remaining = t_cs / 2;
                        current_process->update_completion_status();
                    } else {
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " completed a CPU burst; " << remaining_bursts 
                                << (remaining_bursts == 1 ? " burst" : " bursts") << " to go [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }
                        
                        int io_time = current_process->start_io(current_time + 3);
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " switching out of CPU; blocking on I/O until time " 
                                << current_process->get_io_completion_time()
                                << "ms [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }
                        switching_out = true;
                        context_switch_remaining = t_cs / 2 + 1;
                    }
                }
            }

            current_time++;

            // Check if simulation is complete
            if (all_processes_completed(processes) && ready_queue.empty() && current_process == nullptr && context_switch_remaining == 0) {
                break;
            }
        }

        std::cout << "time " << current_time << "ms: Simulator ended for FCFS [Q " << print_queue(ready_queue) << "]" << std::endl;
    }

    void simulate_sjf(std::vector<Process>& processes, int t_cs, double alpha, double lambda) {
        // Similar structure to FCFS, but use get_next_process_sjf for selecting the next process
        int current_time = 0;
        std::list<Process*> ready_queue;
        Process* current_process = nullptr;
        int context_switch_remaining = 0;
        bool switching_out = false;

        std::cout << "time 0ms: Simulator started for SJF [Q empty]" << std::endl;

        while (true) {

            // Check for new arrivals
            for (Process& p : processes) {
                if (p.get_arrival_time() == current_time) {
                    //std::cout << "ARRIVAL time " << current_time << std::endl;
                    if(p.get_tau() == 0) {
                        p.set_tau(1/lambda);
                    }
                    if(ready_queue.empty()) {
                        ready_queue.push_back(&p);
                    } else {
                        bool added = false;
                        for(std::list<Process*>::iterator itr = ready_queue.begin(); itr != ready_queue.end(); itr++ ) {
                            if(p.get_tau() == (*itr)->get_tau()) {
                                if(p.get_pid() < (*itr)->get_pid()) {
                                    added = true;
                                    ready_queue.insert(itr, &p);
                                    break;
                                }
                            }
                            if(p.get_tau() < (*itr)->get_tau()) {
                                added = true;
                                ready_queue.insert(itr, &p);
                                break;
                            }
                        }
                        if(!added) {
                            ready_queue.push_back(&p);
                        }
                    }
                    if(current_time < 10000) {
                        std::cout << "time " << current_time << "ms: Process " << p.get_pid() 
                            << " (tau " << p.get_tau() << "ms)" << " arrived; added to ready queue [Q " << print_queue(ready_queue) << "]" << std::endl;
                    }
                }
            }

            // Check for I/O completion
            for (Process& p : processes) {
                if (p.is_io_completed(current_time)) {
                    //std::cout << "IO time " << current_time << std::endl;
                    if(p.get_tau() == 0) {
                        p.set_tau(1/lambda);
                    }
                    if(ready_queue.empty()) {
                        ready_queue.push_back(&p);
                    } else {
                        bool added = false;
                        for(std::list<Process*>::iterator itr = ready_queue.begin(); itr != ready_queue.end(); itr++ ) {
                            if(p.get_tau() == (*itr)->get_tau()) {
                                if(p.get_pid() < (*itr)->get_pid()) {
                                    added = true;
                                    ready_queue.insert(itr, &p);
                                    break;
                                }
                            }
                            if(p.get_tau() < (*itr)->get_tau()) {
                                added = true;
                                ready_queue.insert(itr, &p);
                                break;
                            }
                        }
                        if(!added) {
                            ready_queue.push_back(&p);
                        }
                    }
                    
                    if(current_time < 10000) {
                        std::cout << "time " << current_time << "ms: Process " << p.get_pid() 
                            << " (tau " << p.get_tau() << "ms)" << " completed I/O; added to ready queue [Q " << print_queue(ready_queue) << "]" << std::endl;
                    }
                }
            }

            // Handle context switch
            if (context_switch_remaining > 0) {
                context_switch_remaining--;
                if (context_switch_remaining == 0) {
                    // if we are done switching OUT a process (aka CPU is idle)
                    if (switching_out) {
                        switching_out = false;
                        current_process = nullptr;
                    } else if (current_process != nullptr) {
                        // the CPU isn't idle and we start using it
                        int burst_time = current_process->get_next_cpu_burst();
                        if(current_time < 10000) {
                            std::cout << "time " << current_time << "ms: Process " << current_process->get_pid() 
                                << " (tau " << current_process->get_tau() << "ms)" << " started using the CPU for " << burst_time << "ms burst [Q " 
                                << print_queue(ready_queue) << "]" << std::endl;
                        }
                    }
                }
            }

            // Start new process if CPU is idle
            if (current_process == nullptr && !ready_queue.empty() && context_switch_remaining == 0) {
                current_process = ready_queue.front();
                ready_queue.erase(ready_queue.begin());
                context_switch_remaining = t_cs / 2;
            }

            // Process execution
            if (current_process != nullptr && context_switch_remaining == 0) {
                current_process->preempt(1); // decreases the burst time by 1
                if (current_process->get_remaining_time() == 0) {
                    int remaining_bursts = current_process->get_num_bursts() - current_process->get_current_burst_index() - 1;
                    if (remaining_bursts == 0) {
                        std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " terminated [Q " << print_queue(ready_queue) << "]" << std::endl;
                        switching_out = true;
                        context_switch_remaining = t_cs / 2;
                        current_process->update_completion_status();
                    } else {
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " (tau " << current_process->get_tau() << "ms)" << " completed a CPU burst; " << remaining_bursts 
                                << (remaining_bursts == 1 ? " burst" : " bursts") << " to go [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }
                        
                        // recalculate tau
                        int old_tau = current_process->get_tau();
                        current_process->set_tau(std::ceil(alpha * current_process->get_cpu_bursts()[current_process->get_current_burst_index()].first + (1-alpha) * old_tau));

                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Recalculated tau for process " << current_process->get_pid() << ": old tau " << old_tau << "ms ==> new tau " << current_process->get_tau() << "ms [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }

                        int io_time = current_process->start_io(current_time + 3);
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " switching out of CPU; blocking on I/O until time " 
                                << current_process->get_io_completion_time()
                                << "ms [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }
                        switching_out = true;
                        context_switch_remaining = t_cs / 2 + 1;
                    }
                }
            }

            current_time++;

            //Check if simulation is complete
            if (all_processes_completed(processes) && ready_queue.empty() && current_process == nullptr && context_switch_remaining == 0) {
                break;
            }
        }

        std::cout << "time " << current_time << "ms: Simulator ended for FCFS [Q " << print_queue(ready_queue) << "]" << std::endl;
    }

    void simulate_srt(std::vector<Process>& processes, int t_cs, double alpha, double lambda) {
        int current_time = 0;
        std::list<Process*> ready_queue;
        Process* current_process = nullptr;
        int context_switch_remaining = 0;
        bool switching_out = false;

        std::cout << "time 0ms: Simulator started for SRT [Q empty]" << std::endl;

        while (true) {

            // Check for new arrivals
            for (Process& p : processes) {
                if (p.get_arrival_time() == current_time) {
                    //std::cout << "ARRIVAL time " << current_time << std::endl;
                    if(p.get_tau() == 0) {
                        p.set_tau(1/lambda);
                    }
                    if(ready_queue.empty()) {
                        ready_queue.push_back(&p);
                    } else {
                        bool added = false;
                        for(std::list<Process*>::iterator itr = ready_queue.begin(); itr != ready_queue.end(); itr++ ) {
                            if(p.get_tau() == (*itr)->get_tau()) {
                                if(p.get_pid() < (*itr)->get_pid()) {
                                    added = true;
                                    ready_queue.insert(itr, &p);
                                    break;
                                }
                            }
                            if(p.get_tau() < (*itr)->get_tau()) {
                                added = true;
                                ready_queue.insert(itr, &p);
                                break;
                            }
                        }
                        if(!added) {
                            ready_queue.push_back(&p);
                        }
                    }
                    if(current_time < 10000) {
                        std::cout << "time " << current_time << "ms: Process " << p.get_pid() 
                            << " (tau " << p.get_tau() << "ms)" << " arrived; added to ready queue [Q " << print_queue(ready_queue) << "]" << std::endl;
                    }
                }
            }

            // Check for I/O completion
            for (Process& p : processes) {
                if (p.is_io_completed(current_time)) {
                    bool preempt = false;
                    if(p.get_tau() == 0) {
                        p.set_tau(1/lambda);
                    }
                    if(current_process != nullptr && p.get_tau() < current_process->get_tau()) {
                        preempt = true;
                        switching_out = true;
                        context_switch_remaining = t_cs / 2 + 1;
                        ready_queue.push_front(&p);
                    } else if(ready_queue.empty()) {
                        ready_queue.push_back(&p);
                    } else {
                        bool added = false;
                        for(std::list<Process*>::iterator itr = ready_queue.begin(); itr != ready_queue.end(); itr++ ) {
                            if(p.get_tau() == (*itr)->get_tau()) {
                                if(p.get_pid() < (*itr)->get_pid()) {
                                    added = true;
                                    ready_queue.insert(itr, &p);
                                    break;
                                }
                            }
                            if(p.get_tau() < (*itr)->get_tau()) {
                                added = true;
                                ready_queue.insert(itr, &p);
                                break;
                            }
                        }
                        if(!added) {
                            ready_queue.push_back(&p);
                        }
                    }
                    
                    if(current_time < 10000) {
                        if(!preempt) {
                            std::cout << "time " << current_time << "ms: Process " << p.get_pid() 
                            << " (tau " << p.get_tau() << "ms)" << " completed I/O; added to ready queue [Q " << print_queue(ready_queue) << "]" << std::endl;
                        } else {
                            // recalculate old tau for preempted process
                            // old tau - (cpu_burst time - remaining time)
                            int old_tau = current_process->get_tau();
                            current_process->set_tau(old_tau - (current_process->get_cpu_bursts()[current_process->get_current_burst_index()].first - current_process->get_remaining_time()) );
                            std::cout << "time " << current_time << "ms: Process " << p.get_pid() 
                            << " (tau " << p.get_tau() << "ms)" << " completed I/O; preempting " << current_process->get_pid() << " (predicted remaining time " << current_process->get_tau() << "ms)" << " [Q " << print_queue(ready_queue) << "]" << std::endl;
                            // add back curr process according to tau
                            bool added = false;
                            for(std::list<Process*>::iterator itr = ready_queue.begin(); itr != ready_queue.end(); itr++ ) {
                                if(current_process->get_tau() == (*itr)->get_tau()) {
                                    if(current_process->get_pid() < (*itr)->get_pid()) {
                                        added = true;
                                        ready_queue.insert(itr, current_process);
                                        break;
                                    }
                                }
                                if(current_process->get_tau() < (*itr)->get_tau()) {
                                    added = true;
                                    ready_queue.insert(itr, current_process);
                                    break;
                                }
                            }
                            if(!added) {
                                ready_queue.push_back(current_process);
                            }
                            current_process = nullptr;
                        }
                    }
                }
            }

            // Handle context switch
            if (context_switch_remaining > 0) {
                context_switch_remaining--;
                if (context_switch_remaining == 0) {
                    // if we are done switching OUT a process (aka CPU is idle)
                    if (switching_out) {
                        switching_out = false;
                        current_process = nullptr;
                    } else if (current_process != nullptr) {
                        // the CPU isn't idle and we start using it
                        int burst_time = current_process->get_next_cpu_burst();
                        if(current_time < 10000) {
                            std::cout << "time " << current_time << "ms: Process " << current_process->get_pid() 
                                << " (tau " << current_process->get_tau() << "ms)" << " started using the CPU for " << burst_time << "ms burst [Q " 
                                << print_queue(ready_queue) << "]" << std::endl;
                        }
                    }
                }
            }

            // Start new process if CPU is idle
            if (current_process == nullptr && !ready_queue.empty() && context_switch_remaining == 0) {
                current_process = ready_queue.front();
                ready_queue.erase(ready_queue.begin());
                context_switch_remaining = t_cs / 2;
            }

            // Process execution
            if (current_process != nullptr && context_switch_remaining == 0) {
                current_process->preempt(1); // decreases the burst time by 1
                if (current_process->get_remaining_time() == 0) {
                    int remaining_bursts = current_process->get_num_bursts() - current_process->get_current_burst_index() - 1;
                    if (remaining_bursts == 0) {
                        std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " terminated [Q " << print_queue(ready_queue) << "]" << std::endl;
                        switching_out = true;
                        context_switch_remaining = t_cs / 2;
                        current_process->update_completion_status();
                    } else {
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " (tau " << current_process->get_tau() << "ms)" << " completed a CPU burst; " << remaining_bursts 
                                << (remaining_bursts == 1 ? " burst" : " bursts") << " to go [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }
                        
                        // recalculate tau
                        int old_tau = current_process->get_tau();
                        current_process->set_tau(std::ceil(alpha * current_process->get_cpu_bursts()[current_process->get_current_burst_index()].first + (1-alpha) * old_tau));

                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Recalculated tau for process " << current_process->get_pid() << ": old tau " << old_tau << "ms ==> new tau " << current_process->get_tau() << "ms [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }

                        int io_time = current_process->start_io(current_time + 3);
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " switching out of CPU; blocking on I/O until time " 
                                << current_process->get_io_completion_time()
                                << "ms [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }
                        switching_out = true;
                        context_switch_remaining = t_cs / 2 + 1;
                    }
                }
            }

            current_time++;

            //Check if simulation is complete
            if (all_processes_completed(processes) && ready_queue.empty() && current_process == nullptr && context_switch_remaining == 0) {
                break;
            }
        }

        std::cout << "time " << current_time << "ms: Simulator ended for FCFS [Q " << print_queue(ready_queue) << "]" << std::endl;
    }

    void simulate_rr(std::vector<Process>& processes, int t_cs, int t_slice) {
        int current_time = 0;
        std::queue<Process*> ready_queue;
        Process* current_process = nullptr;
        int context_switch_remaining = 0;
        bool switching_out = false;
        int time_slice_remaining = 0;

        std::cout << "time 0ms: Simulator started for RR [Q empty]" << std::endl;

        while (true) {
            // Check for new arrivals and I/O completions
            for (Process& p : processes) {
                if (p.get_arrival_time() == current_time) {
                    ready_queue.push(&p);
                    if(current_time < 10000) {
                        std::cout << "time " << current_time << "ms: Process " << p.get_pid() 
                            << " arrived; added to ready queue [Q " << print_queue(ready_queue) << "]" << std::endl;
                    }
                }
            }
            for (Process& p : processes) {
                if (p.is_io_completed(current_time)) {
                    ready_queue.push(&p);
                    if(current_time < 10000) {
                        std::cout << "time " << current_time << "ms: Process " << p.get_pid() 
                            << " completed I/O; added to ready queue [Q " << print_queue(ready_queue) << "]" << std::endl;
                    }
                }
            }

            // Handle context switch
            if (context_switch_remaining > 0) {
                context_switch_remaining--;
                if (context_switch_remaining == 0) {
                    if (switching_out) {
                        switching_out = false;
                        current_process = nullptr;
                    } else if (current_process != nullptr) {
                        time_slice_remaining = t_slice;
                        if(current_time < 10000) {
                            std::cout << "time " << current_time << "ms: Process " << current_process->get_pid() 
                                    << " started using the CPU for ";
                            if (current_process->get_remaining_time() != current_process->get_total_burst_time()) {
                                std::cout << "remaining " << current_process->get_remaining_time() 
                                        << "ms of " << current_process->get_total_burst_time() << "ms burst";
                            } else {
                                std::cout << current_process->get_remaining_time() << "ms burst";
                            }
                            std::cout << " [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }
                    }
                }
            }

            if (current_process == nullptr && !ready_queue.empty() && context_switch_remaining == 0) {
                current_process = ready_queue.front();
                ready_queue.pop();
                context_switch_remaining = t_cs / 2;
                if (current_process->get_remaining_time() == 0) {
                    current_process->get_next_cpu_burst();
                }
            }

            if (current_process != nullptr && context_switch_remaining == 0) {
                current_process->preempt(1);
                time_slice_remaining--;

                if (current_process->get_remaining_time() == 0) {
                    int remaining_bursts = current_process->get_num_bursts() - current_process->get_current_burst_index() - 1;
                    if (remaining_bursts == 0) {
                        std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " terminated [Q " << print_queue(ready_queue) << "]" << std::endl;
                        current_process->update_completion_status();
                        switching_out = true;
                        context_switch_remaining = t_cs / 2;
                        current_process = nullptr;
                    } else {
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " completed a CPU burst; " << remaining_bursts
                                << (remaining_bursts == 1 ? " burst" : " bursts") << " to go [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }
                        
                        int io_time = current_process->start_io(current_time + 1);
                        int io_completion_time = current_process->get_io_completion_time();
                        
                        bool conflict = false;
                        for (std::vector<Process>::iterator it = processes.begin(); it != processes.end(); ++it) {
                            Process& p = *it;
                            if (&p != current_process && p.get_io_completion_time() == io_completion_time) {
                                conflict = true;
                                break;
                            }
                        }

                        if (conflict) {
                            current_process->set_io_completion_time(io_completion_time + 1);
                            io_completion_time = current_process->get_io_completion_time();
                        }
                                                
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Process " << current_process->get_pid() 
                                << " switching out of CPU; blocking on I/O until time " 
                                << io_completion_time
                                << "ms [Q " << print_queue(ready_queue) << "]" << std::endl;
                        }
                        
                        switching_out = true;
                        context_switch_remaining = t_cs / 2;
                        current_process = nullptr;
                    }
                } else if (time_slice_remaining == 0) {
                    // Time slice expired
                    if (!ready_queue.empty()) {
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Time slice expired; preempting process " << current_process->get_pid() 
                                << " with " << current_process->get_remaining_time() << "ms remaining [Q " 
                                << print_queue(ready_queue) << "]" << std::endl;
                        }
                        ready_queue.push(current_process);
                        switching_out = true;
                        context_switch_remaining = t_cs / 2 + 1;
                        current_process = nullptr;
                    } else {
                        // If ready queue is empty, continue with the current process
                        time_slice_remaining = t_slice;
                        if(current_time < 10000) {
                            std::cout << "time " << current_time + 1 << "ms: Time slice expired; no preemption because ready queue is empty [Q empty]" << std::endl;
                        }
                    }
                }
            }

            current_time++;

            // Check if simulation is complete
            if (all_processes_completed(processes) && ready_queue.empty() && current_process == nullptr && context_switch_remaining == 0) {
                break;
            }
        }

        std::cout << "time " << current_time << "ms: Simulator ended for RR [Q " << print_queue(ready_queue) << "]" << std::endl;
    }

    double estimate_next_burst(double previous_estimate, int actual_burst, double alpha) {
        return alpha * actual_burst + (1 - alpha) * previous_estimate;
    }

    bool all_processes_completed(const std::vector<Process>& processes) {
        for (const Process& p : processes) {
            if (!p.is_completed()) {
                return false;
            }
        }
        return true;
    }

    void reset_processes(std::vector<Process>& processes) {
        for (Process& p : processes) {
            p.reset();
        }
    }

    int get_next_process_srt(const std::vector<Process*>& ready_queue, Process* current_process, double alpha) {
        // Similar to SJF, but also consider the remaining time of the current process
        return 0;
    }

    void print_simulation_results(const std::string& algorithm, const std::vector<Process>& processes, int t_cs) {
        // Print detailed results for each process
    }

    void print_verbose_output(const std::string& event, int time, const Process& process) {
        std::cout << "Time " << time << "ms: " << event << " " << process.get_pid() << std::endl;
    }

    void print_algorithm_start(const std::string& algorithm) {
        std::cout << "Simulating " << algorithm << std::endl;
    }

    void print_algorithm_end(const std::string& algorithm) {
        std::cout << algorithm << " simulation completed\n" << std::endl;
    }

    void calculate_statistics(const std::vector<Process>& processes, int t_cs) {
        // Calculate and store statistics for the current algorithm
    }

    void print_statistics(const std::string& algorithm) {
        // Print the calculated statistics for the given algorithm
    }

int main(int argc, char** argv) {

    // if(argc != 9) {
    //     std::cout << "ERROR: " << "usuage <num_processes> <num_cpu_processes> <seed> <lambda> <upper_bound>" << std::endl;
    //     return 1;
    // }

    /*
        Define n as the number of processes to simulate. Process IDs are assigned a
        two-character code consisting of an uppercase letter from A to Z followed by a number from
        0 to 9. Processes are assigned in order A0, A1, A2, . . ., A9, B0, B1, . . ., Z9.
    */
    int num_processes = atoi(*(argv+1));
    /*
        Define n_cpu as the number of processes that are CPU-bound. For this project, we
        will classify processes as I/O-bound or CPU-bound. The n_cpu CPU-bound processes, when
        generated, will have CPU burst times that are longer by a factor of 4 and will have I/O burst
        times that are shorter by a factor of 8.
    */
    int num_cpu_processes = atoi(*(argv+2));
    /*
        *(argv+3): We will use a pseudo-random number generator to determine the interarrival
        times of CPU bursts. This command-line argument, i.e. seed, serves as the seed for the
        pseudo-random number sequence. To ensure predictability and repeatability, use srand48()
        with this given seed before simulating each scheduling algorithm and drand48() to obtain
        the next value in the range [0.0, 1.0). Since Python does not have these functions, implement
        an equivalent 48-bit linear congruential generator, as described in the man page for these
        functions in C.1
    */
    int seed = atoi(*(argv+3));
    /*
        To determine interarrival times, we will use an exponential distribution, as illustrated in the exp-random.c example. This command-line 
        argument is parameter λ; remember that 1/λ will be the average random value generated, e.g., if λ = 0.01, then the average should
        be appoximately 100. In the exp-random.c example, use the formula shown in the code, i.e., −ln(r)/λ.
    */

    double lambda = 0.001;//atof(*(argv+4));
    //double lambda = atof(*(argv+4));
    /*
        For the exponential distribution, this command-line argument represents the
        upper bound for valid pseudo-random numbers. This threshold is used to avoid values far
        down the long tail of the exponential distribution. As an example, if this is set to 3000, all
        generated values above 3000 should be skipped. For cases in which this value is used in the
        ceiling function (see the next page), be sure the ceiling is still valid according to this upper
        bound.
    */
    int upper_bound = atoi(*(argv+5));
    int t_cs = atoi(*(argv+6));
    double alpha = atof(*(argv+7));
    int t_slice = atoi(*(argv+8));
    Totaller t = Totaller();
    srand48(seed);

    std::cout << "<<< PROJECT PART I" << std::endl;
    std::cout << "<<< -- process set (n=" << num_processes << ") with " << num_cpu_processes; 
    if(num_cpu_processes == 1) {
        std::cout << " CPU-bound process" << std::endl;
    } else {
        std::cout << " CPU-bound processes" << std::endl;
    }
    std::cout << "<<< -- seed=" << seed << "; lambda=" << std::fixed << std::setprecision(6) << lambda << "; bound=" << upper_bound << std::endl;

    std::vector<Process> processes = generate_processes(num_processes, num_cpu_processes, seed, lambda, upper_bound);
    print_processes(processes, t);
    
    std::cout << "\n<<< PROJECT PART II" << std::endl;
    std::cout << std::fixed << std::setprecision(2) << "<<< -- t_cs=" << t_cs << "ms; alpha=" << alpha << "; t_slice=" << t_slice << "ms" << std::endl;
    /*
    print_algorithm_start("FCFS");
    simulate_fcfs(processes, t_cs);
    print_algorithm_end("FCFS");
    reset_processes(processes);

    print_algorithm_start("SJF");
    simulate_sjf(processes, t_cs, alpha, lambda);
    print_algorithm_end("SJF");
    reset_processes(processes);
    /*
    print_algorithm_start("SRT");
    simulate_srt(processes, t_cs, alpha, lambda);
    print_algorithm_end("SRT");
    reset_processes(processes);
    */
    print_algorithm_start("RR");
    simulate_rr(processes, t_cs, t_slice);
    print_algorithm_end("RR");
    
    // Print final statistics
    print_statistics("FCFS");
    print_statistics("SJF");
    print_statistics("SRT");
    print_statistics("RR");

    //file output
    std::ofstream ofile("simout.txt");
    if (!ofile.is_open()) {
        std::cerr << "Error: Unable to open simout.txt for writing." << std::endl;
        return 1;
    }

    ofile << "-- number of processes: " << num_processes << std::endl;
    ofile << "-- number of CPU-bound processes: " << num_cpu_processes << std::endl;
    ofile << "-- number of I/O-bound processes: " << num_processes - num_cpu_processes << std::endl;
    
    double cpu_bound_avg_cpu = 0.0;
    if (t.get_ccbtn() > 0) {
        cpu_bound_avg_cpu = static_cast<double>(t.get_ccbt()) / t.get_ccbtn();
    }

    double io_bound_avg_cpu = 0.0;
    if (t.get_icbtn() > 0) {
        io_bound_avg_cpu = static_cast<double>(t.get_icbt()) / t.get_icbtn();
    }

    double overall_avg_cpu = 0.0;
    if (t.get_ccbtn() + t.get_icbtn() > 0) {
        overall_avg_cpu = static_cast<double>(t.get_ccbt() + t.get_icbt()) / (t.get_ccbtn() + t.get_icbtn());
    }

    double cpu_bound_avg_io = 0.0;
    if (t.get_cibtn() > 0) {
        cpu_bound_avg_io = static_cast<double>(t.get_cibt()) / t.get_cibtn();
    }

    double io_bound_avg_io = 0.0;
    if (t.get_iibtn() > 0) {
        io_bound_avg_io = static_cast<double>(t.get_iibt()) / t.get_iibtn();
    }

    double overall_avg_io = 0.0;
    if (t.get_cibtn() + t.get_iibtn() > 0) {
        overall_avg_io = static_cast<double>(t.get_cibt() + t.get_iibt()) / (t.get_cibtn() + t.get_iibtn());
    }

    ofile << std::fixed << std::setprecision(3);
    ofile << "-- CPU-bound average CPU burst time: " << ceilhelper(cpu_bound_avg_cpu) << " ms" << std::endl;
    ofile << "-- I/O-bound average CPU burst time: " << ceilhelper(io_bound_avg_cpu) << " ms" << std::endl;
    ofile << "-- overall average CPU burst time: " << ceilhelper(overall_avg_cpu) << " ms" << std::endl;
    ofile << "-- CPU-bound average I/O burst time: " << ceilhelper(cpu_bound_avg_io) << " ms" << std::endl;
    ofile << "-- I/O-bound average I/O burst time: " << ceilhelper(io_bound_avg_io) << " ms" << std::endl;
    ofile << "-- overall average I/O burst time: " << ceilhelper(overall_avg_io) << " ms" << std::endl;

    ofile.close();

    return 0;
}
