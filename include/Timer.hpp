#pragma once
#include <chrono>
class Timer {
public:
  using Point = std::chrono::steady_clock::time_point;
  using Duration = std::chrono::steady_clock::duration;
  Timer(Duration duration, Point now);
  void Start(Duration duration, Point now);
  bool IsDone(Point now);

private:
  std::chrono::steady_clock::time_point _finishTime;
};