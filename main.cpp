/*
    authors Vojtech Krejcik (xkrejc68), Jiri Pisk (xpiskj00)
    date 7. 12. 2020
*/

#include <iostream>
#include <iomanip>
#include "simulib.hpp"
#include <typeinfo>
#include <functional>

class PrintPrdelProcess: public Process
{
    public:
    explicit PrintPrdelProcess(Enviroment* env) : Process(env) { }

    void start(void) override
    {
        std::cout << "Prdel" << std::endl;
        this->hand_over(Distribution::normal(5,1),1);
    }
    void print_prdel(void)
    {
        Facility* prdelarna = this->env->get_facility("prdelarna");
        if (prdelarna->occupy()){
            std::cout << "Prdel something" << std::endl;
            this->hand_over(Distribution::normal(5,1),2);
        }else
        {
            prdelarna->enque(this);
            std::cout<<"i am in queue"<<std::endl;
        }
        
    }
    void done(void)
    {
        Facility* prdelarna = this->env->get_facility("prdelarna");
        prdelarna->leave();
        std::cout << "Ahoj ja jsem Dan" << std::endl;
        delete this;
    }
    void next(void) override
    {
        switch (this->next_state)
        {
        case 0:
            start();
            break;
        case 1:
            print_prdel();
            break;
        case 2:
            done();
            break;
        default:
            break;
        }
    }
};

class Generator: public Process
{
    public:
    explicit Generator(Enviroment* env) : Process(env) { }
    void next() override
    {
        start();
    }
    void start() override
    {
        PrintPrdelProcess* proc = new PrintPrdelProcess(this->env);
        this->hand_over(Distribution::normal(10,2),0);
        proc->hand_over(0,0);
    }
};

int main(int argc, char const *argv[])
{
    Enviroment env = Enviroment(200);
    Generator gen = Generator(&env);
    env.add_facility("prdelarna", Facility());
    gen.start();
    env.run();
    
    return 0;
}
