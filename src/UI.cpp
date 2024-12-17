#include "UI.hpp"
#include <SDL2/SDL.h>
#include <iostream>

UI::UI(Keyboard *keyboard, Screen const *screen)
    : _keyboard(keyboard), _screen(screen) {
  (void)_keyboard;
  (void)_screen;
}

void UI::Run() {
  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYDOWN:
        std::cout << e.key.keysym.sym - 'a' << std::endl;
        break;
      }
    }
  }
}