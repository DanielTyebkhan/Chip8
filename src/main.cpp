#include "Emulator.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <iostream>

void SdlErrorOut() {
  std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError();
  exit(1); // NOLINT
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  constexpr int PIXEL_RATIO = 10;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SdlErrorOut();
  }

  SDL_Window *window =
      SDL_CreateWindow("Daniel's Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, Screen::WIDTH * PIXEL_RATIO,
                       Screen::HEIGHT * PIXEL_RATIO, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    SdlErrorOut();
  }
  SDL_Surface *surface = SDL_GetWindowSurface(window);
  // NOLINTNEXTLINE
  SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
  Emulator emulator{};
  emulator.LoadProgram("../ExamplePrograms/ibm_logo.ch8");
  emulator.Run();
  SDL_DestroyWindow(window);

  SDL_Quit();
  return 0;
}