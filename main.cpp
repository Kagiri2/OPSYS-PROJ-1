#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cmath> 
#include "process.h"
#include "next_exp.h"
#include "totaller.h"

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
        p.generate_bursts(seed, upper_bound, lambda);
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
    }
}


int main(int argc, char** argv) {

    if(argc != 6) {
        std::cout << "ERROR: " << "usuage <num_processes> <num_cpu_processes> <seed> <lambda> <upper_bound>" << std::endl;
        return 1;
    }

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

    //file output
    std::ofstream ofile("simout.txt");
    if (!ofile.is_open()) {
        std::cerr << "Error: Unable to open simout.txt for writing." << std::endl;
        return 1;
    }

    ofile << "-- number of processes: " << num_processes << std::endl;
    ofile << "-- number of CPU-bound processes: " << num_cpu_processes << std::endl;
    ofile << "-- number of I/O-bound processes: " << num_processes - num_cpu_processes << std::endl;
    
    double cpu_bound_avg_cpu = t.get_ccbtn() > 0 ? static_cast<double>(t.get_ccbt()) / t.get_ccbtn() : 0.0;
    double io_bound_avg_cpu = t.get_icbtn() > 0 ? static_cast<double>(t.get_icbt()) / t.get_icbtn() : 0.0;
    double overall_avg_cpu = (t.get_ccbtn() + t.get_icbtn()) > 0 ? 
        static_cast<double>(t.get_ccbt() + t.get_icbt()) / (t.get_ccbtn() + t.get_icbtn()) : 0.0;
    
    double cpu_bound_avg_io = t.get_cibtn() > 0 ? static_cast<double>(t.get_cibt()) / t.get_cibtn() : 0.0;
    double io_bound_avg_io = t.get_iibtn() > 0 ? static_cast<double>(t.get_iibt()) / t.get_iibtn() : 0.0;
    double overall_avg_io = (t.get_cibtn() + t.get_iibtn()) > 0 ? 
        static_cast<double>(t.get_cibt() + t.get_iibt()) / (t.get_cibtn() + t.get_iibtn()) : 0.0;

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
