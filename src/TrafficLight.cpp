#include "TrafficLight.h"
#include <iostream>
#include <random>

// Utility function to generate a random real number within an inclusive range
double random_within_range(double min, double max) {
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<double> distr(min, max);
  return distr(generator);
}

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  std::unique_lock<std::mutex> uLck(_mtx);
  _cv.wait(uLck, [this] { return !_queue.empty(); });
  T msg = std::move(_queue.back());
  _queue.pop_back();
  return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  std::lock_guard<std::mutex> lck(_mtx);
  _queue.emplace_back(std::move(msg));
  _cv.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

TrafficLight::~TrafficLight() {}

void TrafficLight::waitForGreen() {
  while (true) {
    TrafficLightPhase phase = _msgQ.receive();
    if (phase == TrafficLightPhase::green)
      return;
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
  std::chrono::time_point<std::chrono::system_clock> lastUpdate =
      std::chrono::system_clock::now();
  long cycleDuration = random_within_range(4.0, 6.0) * 1000; // duration in ms
  while (true) {
    // sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // compute time difference to stop watch
    long timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate)
            .count();

    if (timeSinceLastUpdate >= cycleDuration) {
      TrafficLightPhase phase = (_currentPhase == TrafficLightPhase::red)
                                    ? TrafficLightPhase::green
                                    : TrafficLightPhase::red;
      _currentPhase = phase;
      lastUpdate = std::chrono::system_clock::now();
      _msgQ.send(std::move(phase));
    }
  }
}