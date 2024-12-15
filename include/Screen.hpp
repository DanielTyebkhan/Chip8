#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <array>
#include <cstddef>
class Screen {
public:
  void Clear();

private:
  constexpr static std::size_t WIDTH = 64;
  constexpr static std::size_t HEIGHT = 32;
  std::array<bool, WIDTH * HEIGHT> _pixels;
};

#endif