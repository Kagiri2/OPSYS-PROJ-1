#ifndef CPU_H
#define CPU_H

#include "next_exp.h"

class CPU {
    public:
        int getArrivalTime(int seed, int upper_bound, double lambda);
        int getCPUBurst();
        int getCPUBurstTime(int seed, int upper_bound, double lambda, bool is_cpu_bound);
        int getIOTime(int seed, int upper_bound, double lambda, bool is_cpu_bound) ;
    private:

};

#endif