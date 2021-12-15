#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <vector>

namespace {
  enum class Type {
    Empty = 0,
    Wall,
    Block,
    Bat,
    Ball,
  };

  class Tile {
    public:
    int64_t x;
    int64_t y;
    Type type;

    Tile(int64_t x, int64_t y, int64_t t)
      : x(x)
      , y(y)
      , type(static_cast<Type>(t))
    { }
  };

  using Board = std::vector<Tile>;
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string s;
  aoc::getline(f, s);
  f.close();

  aoc19::Computer c(s, true);
  
  c.initialize();
  size_t blocks_count = 0;
  bool running = true;
  aoc19::InputOutputs outputs;
  do {
    const auto result = c.run(outputs);

    switch (result) {
      case aoc19::HaltCode::HasOutput:
        if (outputs.size() < 3) {
          break;
        }
        while (!outputs.empty()) {
          outputs.pop();
          outputs.pop();
          auto t = outputs.front(); outputs.pop();

          blocks_count += static_cast<Type>(t) == Type::Block;
        }
        break;
      case aoc19::HaltCode::Halt:
        running = false;
        break;
      case aoc19::HaltCode::NeedsInput:
      case aoc19::HaltCode::Error:
        std::cerr << "Unexpected error: " << std::endl;
        std::cerr << c << std::endl;
        return -1;
    }
  } while (running);

  std::cout << "Part 1: " << blocks_count << std::endl;

  // play game
  while (!outputs.empty()) {
    outputs.pop();
  }

  c.initialize();
  c.set_memory(0, 2);
  running = true;
  Board board;
  int64_t bat_x = 0;
  int64_t ball_x = 0;
  int64_t score = 0;
  do {
    const auto result = c.run(outputs);

    switch (result) {
      case aoc19::HaltCode::HasOutput:
        if (outputs.size() < 3) {
          break;
        }
        while (!outputs.empty()) {
          auto x = outputs.front(); outputs.pop();
          auto y = outputs.front(); outputs.pop();
          auto t = outputs.front(); outputs.pop();

          if (x < 0) {
            score = t;
          } else {
            board.emplace_back(x, y, t);

            switch (static_cast<Type>(t)) {
              case Type::Ball:
                ball_x = x;
                break;
              case Type::Bat:
                bat_x = x;
                break;
              default:
                break;
            }
          }
        }
        break;
      case aoc19::HaltCode::Halt:
        running = false;
        break;
      case aoc19::HaltCode::NeedsInput:
        if (bat_x < ball_x) {
          c.set_input(1);
        } else if (bat_x == ball_x) {
          c.set_input(0);
        } else {
          c.set_input( -1 );
        }
        break;
      case aoc19::HaltCode::Error:
        std::cerr << "Unexpected error: " << std::endl;
        std::cerr << c << std::endl;
        return -1;
    }
  } while (running);

  while (!outputs.empty()) {
    std::cout << "Output: " << outputs.front() << std::endl;
    outputs.pop();
  }

  std::cout << "Part 2: " << score << std::endl;

  return 0;
}

