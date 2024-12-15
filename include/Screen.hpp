#ifndef SCREEN_HPP
#define SCREEN_HPP

#include "Types.hpp"
#include <array>
#include <cstddef>
#include <span>
class Screen {
  using Pixel = bool;

public:
  void Clear();

  void Update();

  /**
   * @brief draw sprite to position `x` mod WIDTH, `y` mod HEIGHT. Sprites that
   * exceed the screen's width or height are cut off at the edge (i.e. sprites
   * do not wrap)
   * @return true iff any pixels were erased, i.e. a collision occurred
   */
  bool Draw(Byte x, Byte y, const std::span<Byte> sprite);

  void Display();

  constexpr static std::size_t WIDTH = 64;
  constexpr static std::size_t HEIGHT = 32;

private:
  static void ClearStdout();
  Pixel &PixelAt(std::size_t x, std::size_t y);
  bool _shouldRefresh = true;
  std::array<Pixel, WIDTH * HEIGHT> _pixels = {};
};

#endif