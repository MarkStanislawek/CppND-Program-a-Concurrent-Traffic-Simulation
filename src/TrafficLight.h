#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;

template <class T> class MessageQueue {
public:
  void send(T &&t);
  T receive();

private:
  std::deque<T> _queue;
  std::mutex _mtx;
  std::condition_variable _cv;
};

enum TrafficLightPhase {
  red,
  green,
};

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight();
  ~TrafficLight();

  // getters / setters
  TrafficLightPhase getCurrentPhase();

  // typical behaviour methods
  void waitForGreen();
  void simulate();

private:
  // typical behaviour methods
  void cycleThroughPhases();

  std::condition_variable _condition;
  std::mutex _mutex;
  TrafficLightPhase _currentPhase;
  MessageQueue<TrafficLightPhase> _msgQ;
};

#endif