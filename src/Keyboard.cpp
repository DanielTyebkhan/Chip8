#include "Keyboard.hpp"
#include <cassert>
#include <cstdio>
#include <ios>
#include <iostream>
#include <unistd.h>

// NOLINTBEGIN(*-array-index)
bool Keyboard::IsKeyPressed(std::size_t key) const {
  assert(key < _keyboard.size());
  return _keyboard[key];
}

void Keyboard::SetKeyPressed(std::size_t key, bool isPressed) {
  assert(key < _keyboard.size());
  if (isPressed) {
    for (auto &request : _keyPressRequests) {
      request.set_value(key);
    }
    _keyPressRequests.clear();
  }
  _keyboard[key] = isPressed;
}
// NOLINTEND(*-array-index)

std::future<std::size_t> Keyboard::GetNextKeyPress() {
  _keyPressRequests.emplace_back();
  return _keyPressRequests.back().get_future();
}