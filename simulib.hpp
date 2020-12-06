#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include <functional>
class Distribution
{
    public:
        static double random(void);
        static double exponential(int mean);
        static double normal(int mean, double deviation);
        static double uniform(int lower, int upper);

};
class Enviroment;
class Process
{
    private:
        
    protected:
        Enviroment* env;
        double start_time;
        int next_state;
    public:
        Process(Enviroment* env);
        virtual void start(void) = 0;
        virtual void next(void) = 0;
        void hand_over(double time, int next);
};

class Event
{
    private:
        double time;  
        Process* process;
    public:
        Event(double time,Process* process);
        void execute();
        double get_time();

        bool operator>(Event& other);
        bool operator<(Event& other);
        bool operator==(Event& other);
        bool operator<=(Event& other);
        bool operator>=(Event& other);
};
class Facility
{ 
    private:
        bool occupied = false;
        std::vector<Event> queue;

    public:
        bool is_occupied();
        bool occupy();
        void leave();
        void enque(Process* process);
        void dequeue();
};

class Store
{ 
    private:
        unsigned int capacity;
        unsigned int occupied = 0;
        std::vector<Event> queue;

    public:
        unsigned int available_capacity();
        unsigned int take(unsigned int requirment);
        void give_back(unsigned int requirment);
        void enque(Process* process);
        void dequeue();
};

class Enviroment
{
    private:
        
        double end_time;
        std::vector<Event> event_calendar;
        Event next_event(void);
        std::map<std::string, Facility> facilities;
        std::map<std::string, Store> stores;
    public:
        double current_time = 0;
        Enviroment(double end_time);
        void schedule(Event event);
        void run(void);
        void add_facility(std::string name, Facility fac);
        Facility* get_facility(std::string name);
};
