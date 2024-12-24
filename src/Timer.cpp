#include "Timer.hpp"
#include <chrono>
#include <stdexcept>

Timer::Timer(Clock::duration period) : _period(period) {
  if (_period.count() <= 0) {
    throw std::invalid_argument("period may not be <= 0");
  }
}

bool Timer::IsActive() const noexcept { return _duration.count() > 0; }