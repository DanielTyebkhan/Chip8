#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <future>
#include <vector>

/**
 * @brief thread-safe keyboard implementation
 */
class Keyboard {
public:
  [[nodiscard]] bool IsKeyPressed(std::size_t key) const;
  [[nodiscard]] std::future<std::size_t> GetNextKeyPress() const;
  void SetKeyPressed(std::size_t key, bool isPressed);

private:
  static constexpr std::size_t KEYBOARD_SIZE = 16;
  std::array<std::atomic<bool>, KEYBOARD_SIZE> _keyboard{};
  mutable std::vector<std::promise<std::size_t>> _keyPressRequests;
  std::atomic<bool> _cancelled = false;
};