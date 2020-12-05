#include "simulib.hpp"
#include <ctime>
#include <cmath>
#include <cassert>
#include <algorithm>

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
Event::Event(double time, std::function<void()> operation){
    this->time = time;
    this->operation = operation;
}
void Event::execute()
{
    std::invoke(this->operation);
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

void Process::hand_over(double time, std::function<void()> next)
{
    env->schedule(Event(env->current_time+time,next));
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