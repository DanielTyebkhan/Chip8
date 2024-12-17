#include "Keyboard.hpp"
#include <cassert>

// NOLINTBEGIN(*-array-index)
bool Keyboard::IsKeyPressed(std::size_t key) {
  assert(key < _keyboard.size());
  return _keyboard[key];
}

void Keyboard::SetKeyPressed(std::size_t key, bool isPressed) {
  assert(key < _keyboard.size());
  if (_keyPressEvent.has_value() && isPressed) {
    _keyPressEvent->set_value(key);
  }
  _keyboard[key] = isPressed;
}
// NOLINTEND(*-array-index)

void Keyboard::Reset() {
  for (auto &key : _keyboard) {
    key = false;
  }
}

std::future<std::size_t> Keyboard::GetNextKeyPress() {
  _keyPressEvent = std::promise<std::size_t>();
  return _keyPressEvent->get_future();
}