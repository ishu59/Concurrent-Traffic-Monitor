#include "TrafficLight.h"
#include <chrono>
#include <iostream>
#include <random>
/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  std::unique_lock<std::mutex> light_unq_lock(_tl_mutex);
  _condition.wait(light_unq_lock, [this] { return !_queue.empty(); });
  T msg = std::move(_queue.front());
  _queue.pop_front();
  return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  std::lock_guard<std::mutex> lck(_tl_mutex);
  _queue.push_back(std::move(msg));
  _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() : _currentPhase{TrafficLightPhase::red} {}

void TrafficLight::waitForGreen() {
  while (true) {
//    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (_light_queue.receive() == TrafficLightPhase::green)
      return;
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {

  int min = 4000, max = 6000;
  _currentPhase = TrafficLightPhase ::red;
  auto t1 = std::chrono::high_resolution_clock::now();
  auto random_wait = min + (rand() % static_cast<int>(max - min + 1));
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto t2 = std::chrono::high_resolution_clock::now();
    auto timer =
        std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    if (timer >= random_wait) {

      _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
      _light_queue.send(std::move(_currentPhase));

      random_wait = min + (rand() % static_cast<int>(max - min + 1));
      t1 = std::chrono::high_resolution_clock::now();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

TrafficLightPhase TrafficLight::toggle_light_phase() {
  if (_currentPhase == TrafficLightPhase::red) {
    _currentPhase = TrafficLightPhase ::green;
    return TrafficLightPhase ::green;
  } else {
    _currentPhase = TrafficLightPhase ::red;
    return TrafficLightPhase ::red;
  }
}
