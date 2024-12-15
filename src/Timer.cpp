#include "Timer.hpp"

Timer::Timer(Timer::Duration duration) : _duration(duration) {}

void Timer::Start(Timer::Point now) { _finishTime = now + _duration; }

bool Timer::IsDone(Timer::Point now) {
  if (!_finishTime.has_value()) {
    return false;
  }
  return now >= _finishTime;
}