#include "CPU.h"
#include <cmath>
#include <cstdlib>

int CPU::getArrivalTime(int seed, int upper_bound, double lambda) {
    return floor(next_exp(seed, upper_bound, lambda));
}

int CPU::getCPUBurst() {
    return ceil(drand48() * 32);
}

int CPU::getCPUBurstTime(int seed, int upper_bound, double lambda, bool is_cpu_bound) {
    int cpu_time = next_exp(seed, upper_bound, lambda);
    if (is_cpu_bound) cpu_time *= 4;
    return cpu_time;
}

int CPU::getIOTime(int seed, int upper_bound, double lambda, bool is_cpu_bound) {
    int io_time = ceil(next_exp(seed, upper_bound, lambda));
    if (!is_cpu_bound) io_time *= 8;

    return io_time;
}
