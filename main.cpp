#include <iostream>
#include <iomanip>
#include "simulib.hpp"

int main(int argc, char const *argv[])
{
    double sum = 0;
    for(int i = 0; i<10;i++)
    {
        double res = Distribution::uniform(0.0,10.0);
        sum += res;
        //std::cout << std::fixed << std::setprecision(2) << res << std::endl;
    }
    std::cout << "-------------------------------------------------------" << std::endl << "Mean: " << std::fixed << std::setprecision(2) << sum/10 << std::endl;
    return 0;
}
