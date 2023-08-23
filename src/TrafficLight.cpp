#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function.
    
    // perform container modification under the lock
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_messages.empty(); }); // pass unique lock to condition variable
        
    // retain last element of queue to return later
    T msg = std::move(_messages.back());

    // remove last vector element from queue
    _messages.pop_back();

    return msg;  // will not be copied due to return value optimization (RVO) in C++ 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);

    // Add message to queue
    _messages.push_back(std::move(msg));
    _cond.notify_one();
};

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _type = ObjectType::objectTrafficLight;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.

    while(true)
    {
        TrafficLightPhase new_traffic_light = _traffic_light_queue.receive();
        if (new_traffic_light==TrafficLightPhase::green)
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
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.

    // initialize stop watch
    std::chrono::time_point<std::chrono::system_clock> last_timestamp;
    last_timestamp = std::chrono::system_clock::now();
    
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Create a random cycle duration of value between 4 and 6 seconds
        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_real_distribution<> distr(4.0, 6.0);
        long cycle_duration = distr(eng);

        long time_since_last_cycle = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_timestamp).count();

        if(time_since_last_cycle >= cycle_duration)
        {
            // toggle the current phase of traffic 
            if (_currentPhase == TrafficLightPhase::green)
            {
                _currentPhase = TrafficLightPhase::red;
            }
            else
            {
                _currentPhase = TrafficLightPhase::green;
            }

            // send update method to the message queue using move semantics
            // FP.4b
            // copy for not being moved 
            TrafficLightPhase phase_to_move = _currentPhase;
            _traffic_light_queue.send(std::move(phase_to_move));

            // reset stop watch for next cycle
            last_timestamp = std::chrono::system_clock::now();
        }
    }
}