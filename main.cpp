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
#include <numeric>

/*


                                                                                            /-> find_pokladna  ------------\
                                                                                           /                                \
    get_kosik  ----------------------->start_shoping  ------------------- -------------- ->                                   ---> give_kosik_back
               \                    /                  \                              /   \                                /
                ->fronta na kosik /                     \->get_uzeniny->leave_uzeniny/     \ ->get_samoobsluzna_pokladna  /

*/
class CustomerProcess: public Process
{
    private:
    std::string name_of_pokladna = "";
    
    public:
    explicit CustomerProcess(Enviroment* env) : Process(env) { }

    void start(void) override
    {
        this->hand_over(Distribution::normal(2,1),1);
    }
    void get_kosik(void)
    {
        Store* kosiky = this->env->get_store("kosiky");
        if (kosiky->available_capacity() > 0){
            kosiky->take(1);
            this->hand_over(Distribution::normal(20,1),2);
        }else
        {
            kosiky->enque(this, 1);
        }
        
    }

    void start_shopping()
    {
        
        double decision = Distribution::random();
        //DECISION
        if(decision < 0.7){
            this->hand_over(Distribution::normal(5,1), 3); //jde si nakoupit uzeniny
        }else if(decision < 9){
           this->hand_over(Distribution::normal(5,1), 4); //jde najit pokladnu
        }else{
            this->hand_over(Distribution::normal(5,1), 5); //jde najit samoobslznou pokladnu
        }
    }
    
    void get_uzeniny()
    {
        Store* uzeniny = this->env->get_store("uzeniny");
        if (uzeniny->available_capacity() > 0){
            uzeniny->take(1);
            this->hand_over(Distribution::uniform(2,5),7);// opoustim uzeniny
        }else
        {
            uzeniny->enque(this, 1);
        }
    }

    void leave_uzeniny()
    {
        Store* uzeniny = this->env->get_store("uzeniny");
        uzeniny->give_back(1);
        double decision = Distribution::random();
        //DECISION
        if(decision < 0.7)  this->hand_over(Distribution::normal(5,1), 4); //jde najit pokladnu
        else this->hand_over(Distribution::normal(3,1), 5); //jde najit samoobslznou pokladnu
    }
    
    void find_pokladna()
    {
        Facility* best_pokladna;
        int smallest_queue = 50;
        std::vector<std::string> pokladny = {"pokladna1", "pokladna2", "pokladna3", "pokladna4"};

        // vyber pokladny bez fronty, pripadne najiti pokladny s nejkratsi frontou
        for (std::string pokladna_name : pokladny)
        {
            Facility* pokladna = this->env->get_facility(pokladna_name);
            int queue_size = pokladna->size_of_queue();
            if(queue_size == 0 )
            {
                this->hand_over(Distribution::normal(5,1), 6);//opusti obchod
                this->name_of_pokladna = pokladna_name;
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
        if (s_pokladna->available_capacity() > 0){
            this->name_of_pokladna = "s_pokladna";
            s_pokladna->take(1);
            this->hand_over(Distribution::normal(5,1), 6); //opusti obchod
        }else
        {
            s_pokladna->enque(this, 1);
        }
    }

    void give_kosik_back(void)
    {
        if(this->name_of_pokladna == "s_pokladna"){
            (this->env->get_store(this->name_of_pokladna))->give_back(1);
        }else{
            (this->env->get_facility(this->name_of_pokladna))->leave();
        }
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
// pocet lidi co jdou do fronty na kosik (počet enque u kosiku), celkovy pocet lidi (v generatoru), jak dlouho tam jsou (u enque uložit proces a cas, u deque najit proces odecist cas ulozit do vectoru casu)
class CustomerStatistics:public Statistic
{
    private:
        unsigned int enQ = 0;
        unsigned int customers = 0;
        std::map<Process*,double> enQ_time;
        std::vector<double> times_in_q;
    public:
        void on_enqueue(Process* proc) override
        {
            enQ++;
            enQ_time[proc] = proc->env->current_time;
        }
        void on_dequeue(Event event) override
        {
            double time_enq = enQ_time[event.process];
            enQ_time.erase(event.process);
            times_in_q.push_back(event.process->env->current_time - time_enq);
        }
        void on_generate(void)
        {
            customers++;
        }

        void result(void)
        {
            double mean_q_time = std::accumulate(times_in_q.begin(),times_in_q.end(),0.0) / times_in_q.size();
            std::cout << "CUSTOMER STATISTICS: " << std::endl;
            std::cout << "Do fronty na kosik si stouplo "<< enQ << " zakazniku z celkových " << customers << "." << std::endl;
            std::cout << "Prumerny cas straveny ve fronte na kosiky byl: " << mean_q_time << " minut." << std::endl;
        }
};
class CustomerGenerator: public Process
{
    public:
    CustomerStatistics *stat;

    explicit CustomerGenerator(Enviroment* env) : Process(env) { }
    void next() override
    {
        start();
    }
    void start() override
    {
        CustomerProcess* proc = new CustomerProcess(this->env);
        this->hand_over(Distribution::uniform(1,2),0);
        proc->hand_over(0,0);
        stat->on_generate();
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
    env.add_store("kosiky", Store(25)); 
    env.add_store("uzeniny", Store(2));
    //3 samoobsluzne pokladny jako store
    env.add_store("s_pokladna", Store(3));

    // Add statistics
    CustomerStatistics customerStatistics = CustomerStatistics();
    env.get_store("kosiky")->add_statistic(&customerStatistics);


    // Add generators
    CustomerGenerator gen = CustomerGenerator(&env);
    gen.stat = &customerStatistics;
    //Start generators and run the simulation
    gen.start();



    env.run();
    //Process and print statistics
    customerStatistics.result();
    return 0;
}
