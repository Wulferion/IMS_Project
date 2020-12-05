#include "simulib.hpp"
#include <ctime>
#include <cmath>
#include <cassert>

// Distribution
double Distribution::random(void) 
{
    static uint32_t ix = time(0);
    ix = ix * 69069u + 1u; 
    return ix / ((double)UINT32_MAX + 1.0);
}

double Distribution::exponential(int mean)
{
    return - mean * log(random());
}

double Distribution::normal(int mean, double deviation)
{
    assert(deviation < 0);
    return sqrt(-2*log(random()))*cos(2*3.1415*random())*deviation+mean;
}

double Distribution::uniform(int lower, int upper)
{
    assert(lower <= upper);
    return random() * (upper-lower) + lower;
}
// Event

// Process

// Enviroment
