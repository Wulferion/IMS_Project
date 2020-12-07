/*!
    @authors Vojtech Krejcik (xkrejc68), Jiri Pisk (xpiskj00)
    @date 7. 12. 2020
*/
#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include <functional>

/// @brief class encapsulating static methods for generating distribution
class Distribution
{
    public:
        static double random(void);
        static double exponential(int mean);
        static double normal(int mean, double deviation);
        static double uniform(int lower, int upper);

};
class Enviroment;
/*!
    @class Process
    @brief class for basic process meant to inherited by user of library
    
    Class is supposed to be inherited for creation of process generators a processes itself. Behaviour of process is
    expected to be implementd as finite state machine created by methods and changing 'next_state' attribute. End 
    of each method should be ended with method 'hand_over(double time, int next)' where time is time to, when process
    gets invoked again and 'next' is next_state. See example simulator for better understanding.
*/
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


/*!
    @class Event
    @brief Class for storing reference for process and time when process is suppose to be invoked
*/
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

/*!
    @class Facility
    @brief Implementation of facility and queue for facilit. 
*/
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


/*!
    @class Store
    @brief Implementation of Store with settible capacity and queue
*/
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


/*!
    @class Enviroment
    @brief Class for setting up whole simulation

    Class containing all facilities, store and implementation of 'Event Calendar'
*/
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
        ///pass facility instation and name as string (used for accesing that facility later)
        void add_facility(std::string name, Facility fac);
        ///pass store instation and name as string (used for accesing that store later)
        void add_store(std::string name, Store store);
        Facility* get_facility(std::string name);
        Store* get_store(std::string name);
};
