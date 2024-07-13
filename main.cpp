#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include "process.h"
#include "next_exp.h"
#include "totaller.h"



std::vector<Process> generate_processes(int n, int ncpu, int seed, double lambda, int upper_bound) {
    std::vector<Process> processes;
    CPU cpu;

    for (int i = 0; i < n; i++) {
        std::string pid = std::string(1, 'A' + i / 10) + std::to_string(i % 10);
        bool is_cpu_bound = i < ncpu;
        int arrival_time = cpu.getArrivalTime(seed, upper_bound, lambda);

        Process p(pid, arrival_time, is_cpu_bound);
        p.generate_bursts(seed, upper_bound, lambda);
        processes.push_back(p);
    }

    return processes;
}

void print_processes(const std::vector<Process>& processes, Totaller tot) {
    for (std::vector<Process>::const_iterator process = processes.begin(); process != processes.end(); ++process) {
        bool is_cpu_bound = process->is_cpu_bound_process();
        if (is_cpu_bound) {
            std::cout << "CPU-bound process ";
        } else {
            std::cout << "I/O-bound process ";
        }
        std::cout << process->get_pid() << ": arrival time " << process->get_arrival_time() << "ms; "
                  << process->get_num_bursts() << " CPU bursts:" << std::endl;

        const std::vector<std::pair<int, int>>& cpu_bursts = process->get_cpu_bursts();
        for (std::vector<std::pair<int, int>>::const_iterator burst = cpu_bursts.begin(); burst != cpu_bursts.end(); ++burst) {
            int cpu_burst_time = burst->first;
            int io_burst_time = burst->second;
            std::cout << "==> CPU burst ";
            
            if (is_cpu_bound){
                std::cout << cpu_burst_time + 4 << "ms";
                tot.inc_ccbt(cpu_burst_time + 4);
            } 

            else{
                std::cout << cpu_burst_time + 1 << "ms";
                tot.inc_icbt(cpu_burst_time + 1);
            } 
            
            if (burst != cpu_bursts.end() - 1) { 
                std::cout << " ==> I/O burst ";
                if (is_cpu_bound){
                    std::cout << io_burst_time + 1 << "ms";
                    tot.inc_cibt(io_burst_time + 1);
                } 
                else{
                   std::cout << io_burst_time + 8 << "ms";
                   tot.inc_iibt(io_burst_time + 8); 
                } 
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}


int main(int argc, char** argv) {
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
    double lambda = atof(*(argv+4));
    /*
        For the exponential distribution, this command-line argument represents the
        upper bound for valid pseudo-random numbers. This threshold is used to avoid values far
        down the long tail of the exponential distribution. As an example, if this is set to 3000, all
        generated values above 3000 should be skipped. For cases in which this value is used in the
        ceiling function (see the next page), be sure the ceiling is still valid according to this upper
        bound.
    */
    int upper_bound = atoi(*(argv+5));

    if(argc != 6) {
        std::cout << "ERROR: " << "usuage <num_processes> <num_cpu_processes> <seed> <lambda> <upper_bound>" << std::endl;
    }

    Totaller t;
    srand48(seed);

    std::cout << "<<< PROJECT PART I" << std::endl;
    std::cout << "<<< -- process set (n=" << num_processes << ") with " << num_cpu_processes << " CPU-bound process" << std::endl;
    std::cout << "<<< -- seed=" << seed << "; lambda=" << std::fixed << std::setprecision(6) << lambda << "; bound=" << upper_bound << std::endl;
    std::cout << std::endl;

    std::vector<Process> processes = generate_processes(num_processes, num_cpu_processes, seed, lambda, upper_bound);
    print_processes(processes, t);

    //file output
    ofstream ofile("simout.txt");

    //create simout

    ofile << "-- number of processes: " << num_processes << endl;
    ofile << "-- number of CPU-Bound processes: " << num_cpu_processes << endl;
    ofile << "-- number of I/O-Bound processes: " << num_processes - num_cpu_processes;
    ofile << std::setprecision(3) << "-- CPU-Bound average CPU burst time: " << t.get_ccbt/num_cpu_processes << " ms" <<endl;
    ofile << std::setprecision(3) <<"-- I/O-Bound average CPU burst time: " << t.get_icbt/(num_processes - num_cpu_processes) << " ms" <<endl;
    ofile << std::setprecision(3) <<"-- overall average CPU burst time: " << (t.get_ccbt + t.get_icbt)/num_processes << " ms" << endl;
    ofile << std::setprecision(3) <<"-- CPU-Bound average I/O burst time: " << t.get_cibt/num_cpu_processes << " ms" <<endl;
    ofile << std::setprecision(3) <<"-- I/O-Bound average I/O burst time: " << t.get_iibt/(num_processes - num_cpu_processes) << " ms" <<endl;
    ofile << std::setprecision(3) <<"-- overall average I/O burst time: " << (t.get_cibt + t.get_iibt)/num_processes << " ms" << endl;





    return 0;
}
