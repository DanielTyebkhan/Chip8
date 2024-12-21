#include "DelayTimer.hpp"
#include <chrono>

void DelayTimer::Tick() {
  const auto now = std::chrono::steady_clock::now();
  if (_delay > 0 && now - _lastTick >= DELAY_PERIOD) {
    --_delay;
    _lastTick = now;
  }
}
