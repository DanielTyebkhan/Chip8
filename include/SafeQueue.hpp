#pragma once

#include <mutex>
#include <optional>
#include <queue>
template <typename Contents> class SafeQueue {
public:
  void Enqueue(Contents &&contents) {
    std::unique_lock lock{_mutex};
    _queue.emplace(std::forward<Contents>(contents));
  }

  std::optional<Contents> TryDequeue() {
    std::unique_lock lock{_mutex};
    if (_queue.empty()) {
      return std::nullopt;
    }
    const auto res = std::move(_queue.front());
    _queue.pop();
    return res;
  }

private:
  std::mutex _mutex;
  std::queue<Contents> _queue;
};