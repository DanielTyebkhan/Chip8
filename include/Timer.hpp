#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <vector>
class Timer {
public:
  using Callback = std::function<void(int remainingTicks)>;

  using Clock = std::chrono::steady_clock;
  using TimePoint = Clock::time_point;
  using Duration = Clock::duration;

  Timer(Duration period, bool shouldRepeat, TimePoint (*timeGetter)(),
        Duration initialDuration = Clock::duration::zero());

  void Tick(Clock::time_point current);

  void RegisterCallback(Callback callback) noexcept;

  [[nodiscard]] Clock::duration GetDuration() const noexcept;

  void SetDuration(Clock::duration duration) noexcept;

  Clock::duration GetPeriod();

private:
  Duration _duration;
  Duration _period;
  TimePoint _startTime;
  std::vector<Callback> _callbacks;
  bool _shouldRepeat;
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