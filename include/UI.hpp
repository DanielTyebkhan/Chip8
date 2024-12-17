#pragma once
#include "Keyboard.hpp"
#include "Screen.hpp"
class UI {
public:
  UI(Keyboard *keyboard, Screen const *screen);

private:
  Keyboard *_keyboard;
  Screen const *_screen;
};