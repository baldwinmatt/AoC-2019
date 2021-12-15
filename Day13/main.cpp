#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <vector>

namespace {
  enum class Type {
    Empty = 0,
    Wall,
    Block,
    HorizontalPaddle,
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

  aoc19::Computer c(s);
  
  Board board;
  c.initialize();
  size_t blocks_count = 0;
  do {
    aoc19::InputOutputs outputs;
    const auto result = c.run(outputs);

    while (!outputs.empty()) {
      assert (outputs.size() >= 3);
      
      auto x = outputs.front(); outputs.pop();
      auto y = outputs.front(); outputs.pop();
      auto t = outputs.front(); outputs.pop();

      Tile tile{x, y, t};
      board.push_back(tile);

      blocks_count += tile.type == Type::Block;
    }
    if (result) {
      break;
    }
  } while (false);

  std::cout << "Part 1: " << blocks_count << std::endl;

  return 0;
}

