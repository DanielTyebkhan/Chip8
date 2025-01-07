#include "Timer.hpp"
#include <algorithm>
#include <chrono>
#include <memory>
#include <stdexcept>

Timer::Timer(Clock::duration period, bool shouldRepeat,
             unsigned int initialTicks)
    : _period(period), _remainingTicks(initialTicks),
      _shouldRepeat(shouldRepeat) {
  if (_period.count() <= 0) {
    throw std::invalid_argument("period may not be <= 0");
  }
}

void Timer::RegisterCallback(Callback callback) noexcept {
  _callbacks.push_back(std::move(callback));
}

void Timer::Decrement() {
  --_remainingTicks;
  for (const auto &callback : _callbacks) {
    callback(_remainingTicks);
  }
  if (_remainingTicks == 0 && _shouldRepeat) {
    _remainingTicks = 1;
  }
}

void Timer::Tick(TimePoint currentTime) {
  if (_lastTick.has_value()) {
    const auto diff = *_lastTick - currentTime;
    if (diff >= _period) {
      Decrement();
      _lastTick = currentTime;
    }
  }
  _lastTick = currentTime;
}

std::weak_ptr<Timer>
TimerManager::AddTimer(Timer::Duration period, bool shouldRepeat,
                       unsigned int initialTicks) noexcept {
  _timers.emplace_back(period, shouldRepeat, initialTicks);
  return {_timers.back()};
}

void TimerManager::RemoveTimer(std::weak_ptr<Timer> toRemove) {
  const auto newEnd =
      std::remove_if(_timers.begin(), _timers.end(),
                     [&toRemove](std::shared_ptr<Timer> &timer) {
                       return timer.get() == toRemove.lock().get();
                     });
  if (newEnd != _timers.end()) {
    _timers.erase(newEnd);
  }
}

void TimerManager::Tick() {
  const auto now = Timer::Clock::now();
  for (const auto &timer : _timers) {
    timer->Tick(now);
  }
}
