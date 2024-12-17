#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <future>
#include <optional>

/**
 * @brief thread-safe keyboard implementation
 */
class Keyboard {
public:
  bool IsKeyPressed(std::size_t key);
  void SetKeyPressed(std::size_t key, bool isPressed);
  void Reset();
  std::future<std::size_t> GetNextKeyPress();

private:
  static constexpr std::size_t KEYBOARD_SIZE = 16;
  std::array<std::atomic<bool>, KEYBOARD_SIZE> _keyboard{};
  std::optional<std::promise<std::size_t>> _keyPressEvent;
};