#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

class Timer {
public:
  using Callback = std::function<void(unsigned remainingTicks)>;

  using Clock = std::chrono::steady_clock;
  using TimePoint = Clock::time_point;
  using Duration = Clock::duration;

  Timer(Duration period, bool shouldRepeat, unsigned int initialTicks);

  void Tick(TimePoint current);

  void RegisterCallback(Callback callback) noexcept;

  Clock::duration GetPeriod();

private:
  void Decrement();
  Duration _period;
  std::optional<TimePoint> _lastTick;
  std::vector<Callback> _callbacks;
  unsigned int _remainingTicks;
  bool _shouldRepeat;
};

/**
 * @brief Run multiple timers with a common clock
 *
 */
class TimerManager {
public:
  std::weak_ptr<Timer> AddTimer(Timer::Duration period, bool shouldRepeat,
                                unsigned int initialTicks = 0) noexcept;

  void RemoveTimer(std::weak_ptr<Timer> toRemove);

  void Tick();

private:
  std::vector<std::shared_ptr<Timer>> _timers;
};