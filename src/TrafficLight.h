#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;
enum TrafficLightPhase {red, green};

// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:
	void send(T &&msg);
	T receive(void);

private:
    std::condition_variable _condition;
    std::mutex _mutex;
    std::deque<T> _queue;
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

    
class TrafficLight: public TrafficObject //: public TrafficObject added
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters


    // typical behaviour methods
    void waitForGreen();
    void simulate();
    enum TrafficLightPhase getCurrentPhase();
    
    

private:
    // typical behaviour methods
    void cycleThroughPhases();
    
    // member variables
    TrafficLightPhase _currentPhase;

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.

    std::condition_variable _condition;
    std::mutex _mutex;
    
    //MessageQueue(void) {};
    //MessageQueue(TrafficLightPhase other) mq = new MessageQueue;
	//MessageQueue(MessageQueue const &&other) : _queue(std::move(other._queue)), _mutex(std::move(other._mutex)), _condition(std::move(other._condition)) {};
	std::shared_ptr<MessageQueue<TrafficLightPhase>> _queue{new MessageQueue<TrafficLightPhase>};
	
};

#endif
