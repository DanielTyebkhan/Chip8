#include "UI.hpp"
#include <iostream>
#include <stdexcept>

SdlError::SdlError()
    : std::runtime_error("SdlError: " + std::string(SDL_GetError())) {}

SdlManager::SdlManager(int widthPixels, int heightPixels, Keyboard *keyboard)
    : _keyboard(keyboard) {
  (void)_keyboard;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw SdlError();
  }
  _window = SDL_CreateWindow("Daniel's Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, widthPixels * PIXEL_RATIO,
                             heightPixels * PIXEL_RATIO, SDL_WINDOW_SHOWN);
  if (_window == nullptr) {
    throw SdlError();
  }
  SDL_Surface *surface = SDL_GetWindowSurface(_window);
  // NOLINTNEXTLINE
  SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
}

void SdlManager::Run() {
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

SdlManager::~SdlManager() {
  SDL_DestroyWindow(_window);
  SDL_Quit();
}