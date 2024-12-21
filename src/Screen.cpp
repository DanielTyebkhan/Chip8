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
  static constexpr auto BLOCK = "\u2588";
  static constexpr auto BLANK = " ";
  ClearStdout();
  for (std::size_t y = 0; y < HEIGHT; ++y) {
    for (std::size_t x = 0; x < WIDTH; ++x) {
      std::cout << (PixelAt(x, y) ? BLOCK : BLANK);
    }
    std::cout << '\n';
  }
  std::cout << std::flush;
}

bool Screen::Draw(Byte x, Byte y, const std::span<Byte> sprite) {
  constexpr static unsigned int MSB = 1 << (Constants::BITS_PER_BYTE - 1);
  const auto xBase = x % WIDTH;
  const auto yBase = y % HEIGHT;
  bool changed = false;
  bool collision = false;
  for (std::size_t yOffset = 0; yOffset < sprite.size(); ++yOffset) {
    const auto yCoord = yBase + yOffset;
    if (yCoord >= HEIGHT) {
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
        changed = true;
        const bool pixelTurnedOff = !pixel;
        collision = collision || pixelTurnedOff;
      }
    }
  }
  if (changed) {
    NotifyUpdate();
  }
  return collision;
}

void Screen::RegisterUpdateCallback(UpdateCallback callback) {
  _updateCallbacks.emplace_back(std::move(callback));
}

void Screen::NotifyUpdate() {
  for (auto &callback : _updateCallbacks) {
    callback(_pixels);
  }
}

Screen::Pixel &Screen::PixelAt(std::size_t x, std::size_t y) {
  // NOLINTNEXTLINE
  return _pixels.at(WIDTH * y + x);
}