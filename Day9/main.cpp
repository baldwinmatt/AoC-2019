#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <array>

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);
  std::string s;
  aoc::getline(f, s);
  f.close();

  aoc19::Computer c(s, true);
  aoc19::InputOutputs outputs;
  
  c.initialize();
  c.set_input(1);

  int64_t part1 = 0;
  do {
    const auto result = c.run(outputs);
    if (!outputs.empty()) {
      part1 = outputs.front();
      outputs.pop();
    }
    if (result) {
      break;
    }
  } while (!part1);

  std::cout << "Part 1: " << part1 << std::endl;

  c.initialize();
  c.set_input(2);
  int64_t part2 = 0;
  do {
    const auto result = c.run(outputs);
    if (!outputs.empty()) {
      part2 = outputs.front();
      outputs.pop();
    }
    if (result) {
      break;
    }
  } while (true);

  std::cout << "Part 2: " << part2 << std::endl;

  return 0;
}

