#pragma once

#include <random>
class RandomNumberGenerator {
public:
  /**
   * @brief construct a random number generator with a uniform distribution on
   * [min, max]
   */
  RandomNumberGenerator(int min, int max, int seed);
  int Generate();

private:
  std::mt19937 _rng;
  std::uniform_int_distribution<int> _dist;
};