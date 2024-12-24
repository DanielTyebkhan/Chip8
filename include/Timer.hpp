#pragma once

#include <chrono>
#include <memory>
#include <vector>
class Timer {
public:
  using Clock = std::chrono::steady_clock;
  explicit Timer(Clock::duration period);

  void Tick(Clock::time_point current);

  Clock::duration _duration;

  [[nodiscard]] bool IsActive() const noexcept;

private:
  Clock::duration _period;
};

/**
 * @brief Run multiple timers with a common clock
 *
 */
class TimerManager {
public:
  std::weak_ptr<Timer> AddTimer(Timer::Clock period) noexcept;

  void RemoveTimer(std::weak_ptr<Timer> toRemove);

  void Tick();

private:
  std::vector<std::shared_ptr<Timer>> _timers;
};