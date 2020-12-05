#pragma once
#include <cstdint>
#include <vector>
#include <functional>
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
    private:
        double time;  
        std::function<void()> operation;
    public:
        Event(double time, std::function<void()> operation);
        void execute();
        double get_time();
        bool less_than(Event& e1, Event& e2);
        bool operator>(Event& other);
        bool operator<(Event& other);
        bool operator==(Event& other);
        bool operator<=(Event& other);
        bool operator>=(Event& other);
};
class Enviroment
{
    private:
        
        double end_time;
        std::vector<Event> event_calendar;
        Event next_event(void);
    public:
        double current_time = 0;
        Enviroment(double end_time);
        void schedule(Event event);
        void run(void);
};
class Process
{
    private:
        double start_time;
    
    protected:
        Enviroment* env;
        
    protected:    
        void hand_over(double time, std::function<void()> next);
    
    public:
        Process(Enviroment* env);
        virtual void start(void) = 0;
};



class ProcessGenerator: public Process
{

};

class Facility
{ 

};