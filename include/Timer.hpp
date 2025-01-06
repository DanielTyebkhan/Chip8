#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <vector>
class Timer {
public:
  using Callback = std::function<void(int remainingTicks)>;

  using Clock = std::chrono::steady_clock;

  explicit Timer(Clock::duration period,
                 Clock::duration initialDuration = Clock::duration::zero());

  void Tick(Clock::time_point current);

  void RegisterCallback(Callback callback) noexcept;

  [[nodiscard]] Clock::duration GetDuration() const noexcept;

  void SetDuration(Clock::duration duration) noexcept;

  Clock::duration GetPeriod();

private:
  Clock::duration _duration;
  Clock::duration _period;
  std::vector<Callback> _callbacks;
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