#include "Screen.hpp"
#include "Constants.hpp"
#include "Types.hpp"
#include <iostream>

void Screen::Clear() {
  for (auto &pixel : _pixels) {
    pixel = false;
  }
}

void Screen::ClearStdout() { std::cout << "\033[2J\033[1;1H"; }

void Screen::Display() {
  ClearStdout();
  std::cout << "Updates: " << _updateCount << std::endl;
  for (std::size_t y = 0; y < HEIGHT; ++y) {
    for (std::size_t x = 0; x < WIDTH; ++x) {
      std::cout << (PixelAt(x, y) ? '*' : ' ');
    }
  }
  std::cout << std::flush;
}

void Screen::Update() {
  if (_shouldRefresh) {
    ++_updateCount;
    Display();
    _shouldRefresh = false;
  }
}

bool Screen::Draw(Byte x, Byte y, const std::span<Byte> sprite) {
  constexpr static unsigned int MSB = 1 << (Constants::BITS_PER_BYTE - 1);
  const auto xBase = x % WIDTH;
  const auto yBase = y % HEIGHT;
  _shouldRefresh = false;
  bool collision = false;
  for (std::size_t yOffset = 0; yOffset < sprite.size(); ++yOffset) {
    const auto yCoord = yBase + yOffset;
    if (yCoord > HEIGHT) {
      break;
    }
    const auto spriteRow = sprite[yOffset];
    for (unsigned int bitNum = 0; bitNum < Constants::BITS_PER_BYTE; ++bitNum) {
      const auto xCoord = xBase + bitNum;
      if (xCoord >= WIDTH) {
        break;
      }
      auto &pixel = PixelAt(xCoord, y + yOffset);
      const auto spriteBitSet = static_cast<bool>((MSB >> bitNum) & spriteRow);
      const auto oldPixel = pixel;
      pixel ^= static_cast<int>(spriteBitSet);
      if (oldPixel != pixel) {
        _shouldRefresh = true;
        const bool pixelTurnedOff = !pixel;
        collision = collision || pixelTurnedOff;
      }
    }
  }
  return collision;
}

Screen::Pixel &Screen::PixelAt(std::size_t x, std::size_t y) {
  // NOLINTNEXTLINE
  return _pixels[WIDTH * y + x];
}