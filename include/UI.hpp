#pragma once
#include "Keyboard.hpp"
#include "SafeQueue.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <span>
#include <stdexcept>

struct SdlError : std::runtime_error {
  SdlError();
};

class SdlManager {
  using Frame = std::vector<bool>;

public:
  SdlManager(const SdlManager &) = delete;
  SdlManager(SdlManager &&) = delete;
  SdlManager &operator=(const SdlManager &) = delete;
  SdlManager &operator=(SdlManager &&) = delete;

  SdlManager(int widthPixels, int heightPixels, Keyboard *keyboard);

  void Run();

  void QueueFrame(Frame frame);

  ~SdlManager();

private:
  // frame must be valid
  void RenderFrame(const Frame &toRender);

  void TryRenderFrame();

  void SetKeyStatus(SDL_Keycode key, bool status);

  SDL_Window *_window = nullptr;
  SDL_Surface *_surface = nullptr;
  SDL_Renderer *_renderer = nullptr;
  SDL_Texture *_texture = nullptr;
  std::size_t _screenWidth;
  std::size_t _screenHeight;
  unsigned int _width;
  unsigned int _height;
  Keyboard *_keyboard;
  constexpr static int PIXEL_RATIO = 10;
  SafeQueue<Frame> _frameBuffer;
  std::vector<Uint32> _pixels;
};