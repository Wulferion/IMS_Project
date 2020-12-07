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
#include <string>

class CustomerProcess: public Process
{
    private:
    std::string name_of_poklada = "None";
    
    public:
    explicit CustomerProcess(Enviroment* env) : Process(env) { }

    void start(void) override
    {
        std::cout << "Prijel jsem na parkoviste" << std::endl;
        this->hand_over(Distribution::normal(2,1),1);
        std::cout << "Prdel konec" << std::endl;        
    }
    void get_kosik(void)
    {
        std::cout<<"try to find prdelarna"<<std::endl;
        Store* kosiky = this->env->get_store("kosiky");
        if (kosiky->available_capacity() > 0){
            std::cout << "I have kosik" << std::endl;
            this->hand_over(Distribution::normal(5,1),2);
        }else
        {
            std::cout<<"time to enq"<<std::endl;
            kosiky->enque(this, 1);
            std::cout<<"i am in queue"<<std::endl;
        }
        
    }

    void start_shopping()
    {
        
        //DECISION
        this->hand_over(Distribution::normal(5,1), 3); //jde si nakoupit uzeniny

        this->hand_over(Distribution::normal(5,1), 4); //jde najit pokladnu

        this->hand_over(Distribution::normal(5,1), 5); //jde najit samoobslznou pokladnu

    }
    
    void get_uzeniny()
    {
        Store* uzeniny = this->env->get_store("uzeniny");
        if (uzeniny->available_capacity() > 0){
            this->hand_over(Distribution::normal(5,1),7);///<<<change state
        }else
        {
            uzeniny->enque(this, 1);
        }
    }

    void leave_uzeniny()
    {
        Store* uzeniny = this->env->get_store("uzeniny");
        uzeniny->give_back(1);
        
        //DECISION
        this->hand_over(Distribution::normal(5,1), 4); //jde najit pokladnu

        this->hand_over(Distribution::normal(5,1), 5); //jde najit samoobslznou pokladnu
    }
    
    void find_pokladna()
    {
        Facility* best_pokladna;
        int smallest_queue = 50;
        std::vector<std::string> pokladny = {"pokladna1", "pokladna2", "pokladna3", "pokladna4"};

        // vyber pokladny bez fronty, pripadne najiti pokladny s nejkratsi frontou
        for (std::string pokladna_name : pokladny)
        {
            Facility* pokladna = this->env->get_facility("pokladna_name");
            int queue_size = pokladna->size_of_queue();
            if(queue_size == 0 )
            {
                this->hand_over(Distribution::normal(5,1), 6);
                this->name_of_poklada = pokladna_name;
                return;
            }else if(queue_size < smallest_queue)
            {
                smallest_queue = queue_size;
                best_pokladna = pokladna;
            }
        }
        best_pokladna->enque(this);
    }
    
    void get_samoobsluzna_pokladna()
    {
        Store* s_pokladna = this->env->get_store("s_pokladna");
        if (uzeniny->available_capacity() > 0){
            this->hand_over(Distribution::normal(5,1),7);///<<<change state
        }else
        {
            uzeniny->enque(this, 1);
        }
    }

    void give_kosik_back(void)
    {
        Store* kosiky = this->env->get_store("kosiky");
        kosiky->give_back(1);
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
            get_kosik();
            break;
        case 2:
            start_shopping();
            break;
        case 3:
            get_uzeniny();
            break;
        case 4:
            find_pokladna();
            break;
        case 5:
            get_samoobsluzna_pokladna();
            break;
        case 6:
            give_kosik_back();
            break;
        case 7:
            leave_uzeniny();
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
    // inicializace enviroment s casem 720 (720 minut = 12 hodin)
    Enviroment env = Enviroment(720);
    
    //pomoci facility budeme modelovat 4 otevrenych pokladen
    env.add_facility("pokladna1", Facility());
    env.add_facility("pokladna2", Facility());
    env.add_facility("pokladna3", Facility());
    env.add_facility("pokladna4", Facility());
    //pomoci store budeme modelovat stojan na kosiky a pult na prodej uzenin (1 fronta, 2 lidi obsluhijici zakazniky)
    env.add_store("kosiky", Store(33)); 
    env.add_store("uzeniny", Store(2));


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
