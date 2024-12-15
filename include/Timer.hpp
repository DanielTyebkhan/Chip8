#pragma once
#include <chrono>
#include <optional>
class Timer {
public:
  using Point = std::chrono::steady_clock::time_point;
  using Duration = std::chrono::steady_clock::duration;

  explicit Timer(Duration duration);

  void Start(Point now);

  bool IsDone(Point now);

private:
  Duration _duration;
  std::optional<std::chrono::steady_clock::time_point> _finishTime;
};