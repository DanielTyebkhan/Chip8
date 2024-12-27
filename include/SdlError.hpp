
#include <SDL2/SDL_error.h>
#include <stdexcept>

struct SdlError : std::runtime_error {
  SdlError() : std::runtime_error("SdlError: " + std::string(SDL_GetError())) {}
};