#pragma once

#include "Types.hpp"
#include <chrono>
class DelayTimer {
public:
  void Tick();
  Byte _delay = 0;

private:
  constexpr static std::chrono::nanoseconds DELAY_PERIOD{16666667};
  std::chrono::steady_clock::time_point _lastTick =
      std::chrono::steady_clock::time_point{std::chrono::nanoseconds{0}};
};