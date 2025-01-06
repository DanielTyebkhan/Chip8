#include "Timer.hpp"
#include <chrono>
#include <stdexcept>

Timer::Timer(Clock::duration period, bool shouldRepeat,
             Clock::duration initialDuration)
    : _duration(initialDuration), _period(period), _shouldRepeat(shouldRepeat) {
  if (_period.count() <= 0) {
    throw std::invalid_argument("period may not be <= 0");
  }
}

void Timer::RegisterCallback(Callback callback) noexcept {
  _callbacks.push_back(std::move(callback));
}

void Timer::Tick() {
  if (_duration == Clock::duration::zero()) {
    for (auto &callback : _callbacks) {
      callback();
    }
  }
}
