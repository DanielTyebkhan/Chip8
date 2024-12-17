#include "Random.hpp"

RandomNumberGenerator::RandomNumberGenerator(int min, int max, int seed)
    : _rng(seed), _dist(min, max) {}

int RandomNumberGenerator::Generate() { return _dist(_rng); }