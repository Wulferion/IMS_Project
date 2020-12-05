#pragma once
#include <cstdint>
#include <vector>
class Distribution
{
    public:
    static double random(void);
    static double exponential(int mean);
    static double normal(int mean, double deviation);
    static double uniform(int lower, int upper);

};

class Event
{
  double time;  
};
class Process
{
    
};

class Enviroment
{
    private:
    std::vector<Event> event_calendar;
    public:
    void run(void);
};


class ProcessGenerator: public Process
{

};

class Facility
{

};