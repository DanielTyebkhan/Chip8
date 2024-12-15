#include "Timer.hpp"

Timer::Timer(Timer::Duration duration, Timer::Point now) {
  Start(duration, now);
}

void Timer::Start(Timer::Duration duration, Timer::Point now) {
  _finishTime = now + duration;
}

bool Timer::IsDone(Timer::Point now) { return now >= _finishTime; }