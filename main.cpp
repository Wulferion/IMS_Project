#include <iostream>
#include <iomanip>
#include "simulib.hpp"

void prdel_function(){
    std::cout<<"PRDE:"<<std::endl;
}
int main(int argc, char const *argv[])
{
    Event prdel = Event(23,prdel_function);
    Event prdel2 = Event(22,prdel_function);
    prdel.execute();
    
    std::cout << (prdel == prdel2) << std::endl;

    return 0;
}
