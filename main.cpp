#include <iostream>
#include "next_exp.h"

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
    int lambda = atof(*(argv+4));
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

    std::cout << num_processes << " " << num_cpu_processes << " " << seed << " " << lambda << " " << upper_bound << "" << std::endl;

}  