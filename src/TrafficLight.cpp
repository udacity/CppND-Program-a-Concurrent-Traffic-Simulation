#include <iostream>
#include <random>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> lck(_mutex);
    _msg_condition.wait(lck, [this] { return !_queue.empty(); });

    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a :
    // The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lck(_mutex);
    _queue.push_back(std::move(msg));
    _msg_condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _msgQueue = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        TrafficLightPhase phase = _msgQueue->receive();
        if (phase == TrafficLightPhase::green)
        {
           return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : 
    // Implement the function with an infinite loop that measures the time between two loop cycles 
    // X Toggles the current phase of the traffic light between red and green
    // Sends an update method to the message queue using move semantics.
    // X The cycle duration should be a random value between 4 and 6 seconds. 
    // X Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    while (true)
    {
        // generate wait time between 4 and 6
        float random_s = 4 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0));
        int random_ms = int(random_s * 1000);

        // wait for r seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(random_ms));
        
        // change _currentPhase
        if (_currentPhase == TrafficLightPhase::red)
        {
            _currentPhase = TrafficLightPhase::green;
        }
        else
        {
            _currentPhase = TrafficLightPhase::red;
        }

        // send update message to message queue
        _msgQueue->send(std::move(_currentPhase));
        
        // sleep for 1ms between cycles
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
