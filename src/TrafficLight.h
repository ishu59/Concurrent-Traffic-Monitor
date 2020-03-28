#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;
enum class TrafficLightPhase;
template <class T> class MessageQueue {
public:
  T receive();
  void send(T &&);

private:
  std::deque<T> _queue;
  std::mutex _tl_mutex;
  std::condition_variable _condition;
};

enum class TrafficLightPhase { red, green };

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight();
  // getters / setters
  TrafficLightPhase getCurrentPhase();
  // typical behaviour methods
  void waitForGreen();
  void simulate() override;

private:
  TrafficLightPhase _currentPhase = TrafficLightPhase::red;
  void cycleThroughPhases();
  TrafficLightPhase toggle_light_phase();
  MessageQueue<TrafficLightPhase> _light_queue;
  std::condition_variable _condition;
  std::mutex _mutex;
};

#endif