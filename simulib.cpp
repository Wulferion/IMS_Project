/*
    authors Vojtech Krejcik (xkrejc68), Jiri Pisk (xpiskj00)
    date 7. 12. 2020
*/
#include "simulib.hpp"
#include <ctime>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

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
    assert(deviation > 0);
    return sqrt(-2*log(random()))*cos(2*3.1415*random())*deviation+mean;
}

double Distribution::uniform(int lower, int upper)
{
    assert(lower <= upper);
    return random() * (upper-lower) + lower;
}
// Event
Event::Event(double time, Process* process){
    this->time = time;
    this->process = process;
}
void Event::execute()
{
    process->next();
}

bool Event::operator==(Event& other)
{
    return this->time == other.time;
}

bool Event::operator>(Event& other)
{
    return this->time > other.time;
}

bool Event::operator<(Event& other)
{
    return this->time < other.time;
}

bool Event::operator>=(Event& other)
{
    return this->time >= other.time;
}

bool Event::operator<=(Event& other)
{
    return this->time <= other.time;
}

double Event::get_time()
{
    return this->time;
}
// Process

Process::Process(Enviroment* env)
{
    this->env = env;
}

void Process::hand_over(double time,int next)
{
    for (auto const& stat : this->statistics)
    {
        stat.second->on_process_handover(this,time);
    }

    this->next_state = next;
    env->schedule(Event(env->current_time+time, this));
    return;
}

void Process::add_statistic(std::string name,Statistic* statistic)
{
    this->statistics[name] = statistic;
}

// Enviroment

Enviroment::Enviroment(double duration)
{
    this->end_time = duration;
}

Event Enviroment::next_event(void)
{
    Event event = event_calendar.front();
    event_calendar.erase(event_calendar.begin());
    return event;
}

void Enviroment::schedule(Event event)
{
    for (Statistic* stat : this->statistics)
    {
        stat->on_event_schedule(event,current_time);
    }

    if (event.get_time() > this->end_time) return;
    for(unsigned int i = 0;i<event_calendar.size();i++)
    {
        if (event_calendar[i] > event)
        {
            event_calendar.emplace(event_calendar.begin()+i,event);
            return;
        }
    }
    event_calendar.push_back(event);
}

void Enviroment::run(void)
{
    while(!this->event_calendar.empty())
    {
        Event nevent = next_event();
        this->current_time = nevent.get_time();
        for (Statistic* stat : this->statistics)
        {
                stat->on_event_execute(nevent,current_time);
        }
        nevent.execute();
    }
}

void Enviroment::add_facility(std::string name, Facility fac)
{
    this->facilities[name] = fac;
}

void Enviroment::add_store(std::string name, Store store)
{
    this->stores[name] = store;
}

void Enviroment::add_statistic(Statistic* stat)
{
    this->statistics.push_back(stat);
}


Facility* Enviroment::get_facility(std::string name)
{
    if(facilities.find(name) == facilities.end()) return NULL;
    return &facilities[name];
}

//Facitity
bool Facility::is_occupied(){
    return this->occupied;
}

bool Facility::occupy()
{
    for (Statistic* stat : this->statistics)
    {
        stat->on_facility_occupy(!this->occupied);
    }

    if( this->occupied == true) return false;
    this->occupied = true;
    return true;
    
}
void Facility::leave()
{
    for (Statistic* stat : this->statistics)
    {
        stat->on_facility_leave();
    }
    this->occupied = false;
    this->dequeue();
}

void Facility::enque( Process* process)
{
    for (Statistic* stat : this->statistics)
    {
        stat->on_enqueue(process);
    }

    this->queue.push_back(Event(0, process));
}

void Facility::dequeue()
{
    
    if (queue.empty()) return;
    Event event = queue.front();
    queue.erase(queue.begin());

    for (Statistic* stat : this->statistics)
    {
        stat->on_dequeue(event);
    }

    event.execute();
}
int Facility::size_of_queue()
{
    return this->queue.size();
}

void Facility::add_statistic(Statistic* stat)
{
    this->statistics.push_back(stat);
}
//Store
Store::Store(int capacity_req)
{
    this->capacity = capacity_req;
}

Store* Enviroment::get_store(std::string name)
{
    return &stores[name];
}

unsigned int Store::available_capacity()
{
    return (this->capacity - this->occupied);
}

int Store::size_of_queue()
{
    return this->queue.size();
}


unsigned int Store::take(unsigned int requirment)
{
    for (Statistic* stat : this->statistics)
    {
        stat->on_store_take(this->available_capacity() >= requirment,requirment,this->available_capacity());
    }

    if( this->available_capacity() < requirment) return false;
    this->occupied += requirment;
    return true;
    
}
void Store::give_back(unsigned int requirment)
{
    for (Statistic* stat : this->statistics)
    {
        stat->on_store_give_back(requirment,this->available_capacity());
    }

    this->occupied -= requirment;
    this->dequeue();
}


void Store::enque( Process* process, int required = 1)
{
    // atribut time of event isnt needed, so we put in it number of required capcity
    this->queue.push_back(Event(required, process));
    for (Statistic* stat : this->statistics)
    {
        stat->on_enqueue(process);
    }
}


void Store::dequeue(){

    if (queue.empty()) return;
    Event event = queue.front();
    // if capacity requirment of process isnt fullfilled, dequeuing is canceled
    if( this->available_capacity() < event.get_time()) return;

    queue.erase(queue.begin());

    for (Statistic* stat : this->statistics)
    {
        stat->on_dequeue(event);
    }

    event.execute();
}

void Store::add_statistic(Statistic* stat)
{
    this->statistics.push_back(stat);
}

//Statistics

void RuntimeStat::print_stat(void)
{
    std::cout << "RUNTIME STATISTIC: " << std::endl << std::endl;
    std::cout << "Events scheduled: " << scheduled << "          " << "Events executed: " << std::endl << std::endl;
    std::cout << "Simulation time step: " << std::endl;
    for (double step : this->steps)
    {
        std::cout << step << "|";
    }
}

void RuntimeStat::on_event_schedule(Event event,double time)
{
    scheduled++;
}

void RuntimeStat::on_event_execute(Event event,double time)
{
    executed++;
    steps.push_back(time);
}