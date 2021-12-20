#include "aoc/helpers.h"
#include <array>
#include <vector>

namespace {
  constexpr size_t LayerWidth = 25;
  constexpr size_t LayerHeight = 6;
  constexpr size_t LayerSize = LayerWidth * LayerHeight;

  using Image = std::array<int, LayerSize>;
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  char c;
  size_t p = 0;
  Image sif;
  size_t zeroes = 0;
  size_t ones = 0;
  size_t twos = 0;
  size_t least_zeroes = SIZE_MAX;
  size_t score = 0;
  bool first = true;
  while ((c = f.get()) && f.good()) {
    if (first || sif[p] == 2) {
      sif[p] = c - '0';
    }
    p++;
    
    switch (c) {
      case '0':
        zeroes++;
        break;
      case '1':
        ones++;
        break;
      case '2':
        twos++;
        break;
      default:
        break;
    }

    if (p == LayerSize) {
      first = false;
      if (zeroes < least_zeroes) {
        least_zeroes = zeroes;
        score = ones * twos;
      }

      p = 0;
      ones = 0;
      twos = 0;
      zeroes = 0;
    }
  }

  std::cout << "Part 1: " << score << std::endl;
  std::cout << std::endl;

  p = 0;
  for (auto px : sif) {
    if (px == 0) {
      std::cout << " ";
    } else {
      std::cout << "X";
    }
    p++;
    if (p % LayerWidth == 0) {
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;
  return 0;
}

