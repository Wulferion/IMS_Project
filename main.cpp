/*
    authors Vojtech Krejcik (xkrejc68), Jiri Pisk (xpiskj00)
    date 7. 12. 2020
*/
/** \example main.cpp
 * This is an example of how to use our simulation library simulib.
 * See the code for more details.
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
        std::cout << "Prdel zacatek" << std::endl;
        this->hand_over(Distribution::normal(5,1),1);
        std::cout << "Prdel konec" << std::endl;        
    }
    void print_prdel(void)
    {
        std::cout<<"try to find prdelarna"<<std::endl;
        Facility* prdelarna = this->env->get_facility("prdelarna");
        std::cout<<"prdelarna found"<<std::endl;
        if (prdelarna->occupy()){
            std::cout << "Prdel something" << std::endl;
            this->hand_over(Distribution::normal(5,1),2);
        }else
        {
            std::cout<<"time to enq"<<std::endl;
            prdelarna->enque(this);
            std::cout<<"i am in queue"<<std::endl;
        }
        
    }
    void done(void)
    {
        std::cout<<"Dan prichazi"<<std::endl;
        Facility* prdelarna = this->env->get_facility("prdelarna");
        std::cout<<"Dan odchazi z prdelarny"<<std::endl;
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

class Enviroment_stats: public Statistic
{
    public:
        int scheduled = 0;
        int executed = 0;

    void on_event_schedule(Event event) override
    {
        this->scheduled++;
    }
    void on_event_execute(Event event) override
    {
        this->executed++;
    }
    void result(void)
    {
        std::cout << "Scheduled events: " << this->scheduled << std::endl << "Executed events: " << this->executed << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    // Init enviroment
    Enviroment env = Enviroment(2000);
    // Add generators
    Generator gen = Generator(&env);
    // Add facilities
    env.add_facility("prdelarna",Facility());
    // Add statistics
    Enviroment_stats stat1 = Enviroment_stats();
    env.add_statistic(&stat1);
    //Start generators and run the simulation
    gen.start();
    env.run();
    //Process and print statistics
    stat1.result();
    return 0;
}


/*/V enviroment

map <string name,Statistic *stat>;

//V mainu
class ProceessTimes: public Staticstic
{
    void do_something(void) override
    {

    }
}

ProceessTimes proctimestat;
env -> addstat("name of statistics",&stat)
env.run()

cout << time in processsed << endl
proctimestat.histogram();

//Kdekoliv v env
for each statistic in mapstat
    stistic->do_something
/*/