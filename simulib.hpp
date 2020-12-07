/*
    authors Vojtech Krejcik (xkrejc68), Jiri Pisk (xpiskj00)
    date 7. 12. 2020
*/
#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include <functional>

/// @brief Static class for generating distribution
class Distribution
{
    public:
        static double random(void);
        static double exponential(int mean);
        static double normal(int mean, double deviation);
        static double uniform(int lower, int upper);

};
class Enviroment;
class Statistic;
/*!
    @class Process
    @brief class for basic process meant to be inherited from
    
    This class is supposed to be inherited from for creation of process generators a processes themselves. Behaviour of the process is
    expected to be implementd as finite state machine created by methods and changing 'next_state' attribute. End 
    of each method should be ended with method 'hand_over(double time, int next)' where time is time to, when process
    gets invoked again and 'next' is next_state. See example simulator for better understanding.
*/
class Process
{
    private:
        
    protected:
        double start_time;
        int next_state;
        std::map<std::string,Statistic*> statistics;
    public:
        Enviroment* env;
        /// Constructor for porcess, class, requires initilized enviroment
        Process(Enviroment* env);
        /// Registers a new statistic for process to use
        void add_statistic(std::string name,Statistic* statistic);
        virtual void start(void) = 0;
        virtual void next(void) = 0;
        /// creates event in calendar to 'current_time' + 'time' and sets state if process on 'next'
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
    public:
        Process* process;
        /// Creates event with 'current_time' + 'time' and reference to process
        Event(double time, Process* process);
        /// Invokes process from event according to his state
        void execute();
        /// returns event time
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
        std::vector<Statistic*> statistics;
    public:
        /// True if facility is occupied and false, if not
        bool is_occupied();
        /// Sets occupied on true and returns true, if facility is already occupied returns false
        bool occupy();
        /// Sets occupied on false -> is not accesible to others, user is required to call 'leave' later, or facility wont be accesible
        void leave();
        /// process is put in queue and is invoked when its time
        void enque(Process* process);
        /// process on begining of queue is removed and invoked
        void dequeue();
        /// returns size of queue
        int size_of_queue();
        /// Registers a new statistic for facility to use
        void add_statistic(Statistic* statistic);
};


/*!
    @class Store
    @brief Implementation of Store with settible capacity and queue
*/
class Store
{ 
    private:
        unsigned int capacity = 0;
        unsigned int occupied = 0;
        std::vector<Event> queue;
        std::vector<Statistic*> statistics;
    public:
        Store(){};
        Store(int capacity_req);
        /// returns available capacity
        unsigned int available_capacity();
        /// takes capacity if available
        unsigned int take(unsigned int requirment);
        /// gives back capacity
        void give_back(unsigned int requirment);
        /// process is putted in queue and invoked when its time
        void enque(Process* process, int required);
        /// process from front of queue is waiting till his requirment is possible to satisfy, all processes behind him, are waiting too (no overtaking)
        void dequeue();
        /// returns size of queue
        int size_of_queue();
        /// Registers a new statistic for store to use
        void add_statistic(Statistic* statistic);
};


/*!
    @class Enviroment
    @brief Class for setting up whole simulation

    Class containing all facilities, store and implementation of 'Event Calendar'
*/
class Enviroment
{
    private:
        std::vector<Statistic*> statistics;
        double end_time;
        std::vector<Event> event_calendar;
        Event next_event(void);
        std::map<std::string, Facility> facilities;
        std::map<std::string, Store> stores;
    public:
        double current_time = 0;
        Enviroment(double end_time);
        /// puts event in calendar (time is spsecified, by event)
        void schedule(Event event);
        /// main simulation loop
        void run(void);
        /// pass facility instation and name as string (used for accesing that facility later)
        void add_facility(std::string name, Facility fac);
        /// pass store instation and name as string (used for accesing that store later)
        void add_store(std::string name, Store store);
        /// gets facility by name (as string)
        Facility* get_facility(std::string name);
        /// gets store by name (as string)
        Store* get_store(std::string name);
        /// Registers a new statistic for enviroment to use
        void add_statistic(Statistic* statistic);
};

/*!
    @class Statistic
    @brief Base class for creating statistics. 

    Base class for statistics which provides entry points to the inner works of other classes with its methods.
*/
class Statistic
{
    public:
        /// Override with your implementation. Called on every schedule call in registered enviroment. The event parameter is filled with the copy of the event scheduled.
        virtual void on_event_schedule(Event event,double time){};
        /// Override with your implementation. Called on every event execute call in registered enviroment. The event parameter is filled with the copy of the event executed.
        virtual void on_event_execute(Event event,double time){};
        /// Override with your implementation. Called on every handover call in registered process. Reference to the process is given as an argument
        virtual void on_process_handover(Process* proc,double time){};
        /// Override with your implementation. Called on every occupy call in registered facility.
        virtual void on_facility_occupy(bool occupied){};
        /// Override with your implementation. Called on every leave call in registered facility.
        virtual void on_facility_leave(void){};
        /// Override with your implementation. Called on every take call in registered store.
        virtual void on_store_take(bool occupied,unsigned int requirement, unsigned int available){};
        /// Override with your implementation. Called on every give_back call in registered store.
        virtual void on_store_give_back(unsigned int value, unsigned int available){};
        /// Override with your implementation. Called on every enqueue call in registered facility or store. Reference to the process is given as an argument
        virtual void on_enqueue(Process* proc){};
        /// Override with your implementation. Called on every dequeue call in registered facility or store. Reference to the event is given as an argument
        virtual void on_dequeue(Event event){};
};

class RuntimeStat: public Statistic
{
    private:
        // Events scheduled, Events executed, steps
        std::vector<double> steps;
        unsigned int scheduled = 0;
        unsigned int executed = 0; 
    public:
        void print_stat(void);
        void on_event_schedule(Event event,double time) override;
        void on_event_execute(Event event,double time) override;
};