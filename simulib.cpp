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
    this->next_state = next;
    env->schedule(Event(env->current_time+time, this));
    return;
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
    //auto bigger = std::upper_bound(begin(event_calendar), end(event_calendar), event);

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
        nevent.execute();
    }
}

void Enviroment::add_facility(std::string name, Facility fac)
{
    this->facilities[name] = fac;
}

Facility* Enviroment::get_facility(std::string name)
{
    return &facilities[name];
}

bool Facility::is_occupied(){
    return this->occupied;
}

bool Facility::occupy()
{
    if( this->occupied == true) return false;
    this->occupied = true;
    return true;
    
}
void Facility::leave()
{
    this->occupied = false;
}

void Facility::enque( Process* process)
{
    this->queue.push_back(Event(0, process));
}

void Facility::dequeue(){
    Event event = queue.front();
    queue.erase(queue.begin());
    event.execute();
}