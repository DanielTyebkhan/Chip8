#pragma once
#include "Keyboard.hpp"
#include "Screen.hpp"
class UI {
public:
  UI(Keyboard *keyboard);
  void Run();

private:
  Keyboard *_keyboard;
};