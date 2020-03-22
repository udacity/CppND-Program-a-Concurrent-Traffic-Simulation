#include <iostream>
#include <random>
#include <thread>
#include "TrafficLight.h"
#include <future>

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> ulock(_mutex);
    _condition.wait(ulock,[this]{return !_messages.empty();});
    T msg = std::move(_messages.back());
    _messages.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> ulock(_mutex);
    _messages.push_back(std::move(msg));
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. 
    //To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

    //Random Machine generating a random cycle duration between 4-6s
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<long long> dist(4.0,6.0);
    long long cycleDuration = dist(mt);

    //Set up initial time point 
    std::chrono::time_point<std::chrono::system_clock> startTimePoint;
    startTimePoint = std::chrono::system_clock::now();

    //Measuring cycles 
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        long long elaspedTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()- startTimePoint).count();
        if(elaspedTime >= cycleDuration){
            //toggle traffic light phase 
            _currentPhase = (_currentPhase==TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;

            //send message to alert the data (red/green) is available
            std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send,&_messageQueue,std::move(_currentPhase));

            //restart prevTimePoint
            startTimePoint = std::chrono::system_clock::now();
        }
    }

}

