#pragma once
#include "Keyboard.hpp"
#include <SDL2/SDL.h>
#include <stdexcept>

struct SdlError : std::runtime_error {
  SdlError();
};

class SdlManager {
public:
  SdlManager(const SdlManager &) = delete;
  SdlManager(SdlManager &&) = delete;
  SdlManager &operator=(const SdlManager &) = delete;
  SdlManager &operator=(SdlManager &&) = delete;

  SdlManager(int widthPixels, int heightPixels, Keyboard *keyboard);

  void Run();

  ~SdlManager();

private:
  SDL_Window *_window = nullptr;
  Keyboard *_keyboard;
  constexpr static int PIXEL_RATIO = 10;
};