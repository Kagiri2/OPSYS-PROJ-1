#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "next_exp.h"

int next_exp(int seed, int upper_bound, double lambda)
{
    double sum = 0;
    int iterations = 1; 

    for ( int i = 0 ; i < iterations ; i++ )
    {
        double r = drand48();  /* uniform dist [0.00,1.00) -- also see random() */
        /* generate the next pseudo-random value x */
        double x = -log( r ) / lambda;   /* log() is natural log (see man page) */

        /* skip values that are far down the "long tail" of the distribution */
        if ( x > upper_bound ) { i--; continue; }

        sum += x;
    }

    double avg = sum / iterations;
    return avg;
}