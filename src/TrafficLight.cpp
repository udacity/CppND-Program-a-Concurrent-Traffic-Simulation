#include <iostream>
#include <future>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> lck(mtx);
    _cond.wait(lck, [this](){return !_queue.empty();});
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lck(mtx);
    this ->_queue.emplace_back(std::move(msg));
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    traffic_light_queue = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {       
        auto receive_msg = traffic_light_queue->receive();
        if (receive_msg == TrafficLightPhase::green){
            return;
        }
    }
    
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a 
    //thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 

    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}
long TrafficLight::generateNewCycleIterval(){
    srand(time(0));
    return (rand() % 4000 + 2000);
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
{
    long intervall = generateNewCycleIterval();
    std::future<void> future;
    std::chrono::time_point<std::chrono::system_clock> lastUpdate{std::chrono::system_clock::now()};

    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        std::unique_lock<std::mutex> lck(_mutex);
        if (elapsed >= intervall) {
            if (_currentPhase == TrafficLightPhase::red) {
                _currentPhase = TrafficLightPhase::green;
                future = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, traffic_light_queue, _currentPhase);
                intervall = generateNewCycleIterval();
                future.wait();
                lastUpdate = std::chrono::system_clock::now();
            } else {
                _currentPhase = TrafficLightPhase::red;
                intervall = generateNewCycleIterval();
                lastUpdate = std::chrono::system_clock::now();
            }
        }
    }
    

}