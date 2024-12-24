#include "UI.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <stdexcept>

SdlError::SdlError()
    : std::runtime_error("SdlError: " + std::string(SDL_GetError())) {}

SdlManager::SdlManager(int widthPixels, int heightPixels, Keyboard *keyboard)
    : _screenWidth(static_cast<std::size_t>(widthPixels * PIXEL_RATIO)),
      _screenHeight(static_cast<std::size_t>(heightPixels * PIXEL_RATIO)),
      _width(widthPixels), _height(heightPixels), _keyboard(keyboard) {
  _pixels.reserve(widthPixels * heightPixels);
  (void)_keyboard;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw SdlError();
  }
  _window =
      SDL_CreateWindow("Daniel's Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, static_cast<int>(_screenWidth),
                       static_cast<int>(_screenHeight), SDL_WINDOW_SHOWN);
  if (_window == nullptr) {
    throw SdlError();
  }
  _surface = SDL_GetWindowSurface(_window);
  // NOLINTNEXTLINE
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  _texture =
      SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888,
                        SDL_TEXTUREACCESS_STREAMING, widthPixels, heightPixels);
}

void SdlManager::TryRenderFrame() {
  const auto frame = _frameBuffer.TryDequeue();
  if (!frame.has_value() || frame->size() != _width * _height) {
    return;
  }
  RenderFrame(*frame);
}

void SdlManager::RenderFrame(const Frame &toRender) {
  constexpr static Uint32 PIXEL_ON = 0xFFF;
  constexpr static Uint32 PIXEL_OFF = 0x000;
  std::transform(toRender.begin(), toRender.end(), _pixels.begin(),
                 [](bool on) { return on ? PIXEL_ON : PIXEL_OFF; });
  SDL_UpdateTexture(_texture, nullptr, _pixels.data(),
                    static_cast<int>(_width * sizeof(Uint32)));
  SDL_Rect destRect = {0, 0, static_cast<int>(_screenWidth),
                       static_cast<int>(_screenHeight)};
  SDL_RenderCopy(_renderer, _texture, nullptr, &destRect);
  SDL_RenderPresent(_renderer);
}

void SdlManager::QueueFrame(Frame frame) {
  _frameBuffer.Enqueue(std::move(frame));
}

void SdlManager::SetKeyStatus(SDL_Keycode key, bool status) {
  static constexpr auto keys = std::array{
      SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
      SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v,
  };
  const auto *const keyIter = std::find(keys.begin(), keys.end(), key);
  if (keyIter != keys.end()) {
    _keyboard->SetKeyPressed(keyIter - keys.begin(), status);
  }
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
        SetKeyStatus(e.key.keysym.sym, true);
        break;
      case SDL_KEYUP:
        SetKeyStatus(e.key.keysym.sym, false);
        break;
      }
    }
    TryRenderFrame();
  }
}

SdlManager::~SdlManager() {
  SDL_DestroyWindow(_window);
  SDL_Quit();
}