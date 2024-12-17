#include "Keyboard.hpp"
#include <cassert>
#include <cstdio>
#include <unistd.h>

// NOLINTBEGIN(*-array-index)
bool Keyboard::IsKeyPressed(std::size_t key) const {
  assert(key < _keyboard.size());
  return _keyboard[key];
}

void Keyboard::SetKeyPressed(std::size_t key, bool isPressed) {
  assert(key < _keyboard.size());
  if (_keyPressEvent.has_value() && isPressed) {
    _keyPressEvent->set_value(key);
    _keyPressEvent.reset();
  }
  _keyboard[key] = isPressed;
}
// NOLINTEND(*-array-index)

std::future<std::size_t> Keyboard::GetNextKeyPress() const {
  _keyPressEvent = std::promise<std::size_t>();
  return _keyPressEvent->get_future();
}