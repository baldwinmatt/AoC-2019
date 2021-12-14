#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <array>
#include <map>
#include <set>

namespace {

  enum class Color {
    Black = 0,
    White,
  };

  using Point = std::pair<int, int>;
  using Grid = std::map<Point, Color>;

  void DisplayGrid(const Grid& g, const Point& tl, const Point& br) {
    const size_t width = br.first - tl.first;
    const size_t height = br.second - tl.second;

    std::cout << "Grid: { " << tl.first << ", " << tl.second << " } -> { " << br.first << ", " << br.second << " }" << std::endl;

    for (size_t y = 0; y <= height; y++) {
      for (size_t x = 0; x <= width; x++) {
        Point p{x + tl.first, y + tl.second};
        const auto c = g.find(p);
        if (c != g.end()) {
          if (c->second == Color::White) {
            std::cout << "#";
          } else {
            std::cout << ".";
          }
        } else {
          std::cout << " ";
        }
      }
      std::cout << std::endl;
    }
  }

  enum class Direction {
    Up = 0,
    Left,
    Down,
    Right,
  };

  const std::map<Direction, Point> Directions = {
      { Direction::Right, {1,0} },
      { Direction::Left, {-1,0} },
      { Direction::Up, {0,1} },
      { Direction::Down, {0,-1} },
  };

  const auto turn_left = [](Direction dir) {
    const int val = static_cast<int>(dir);
    return static_cast<Direction>((val + 1) & 3);
  };

  const auto turn_right = [](Direction dir) {
    const int val = static_cast<int>(dir);
    return static_cast<Direction>((val + 3) & 3);
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string s;
  aoc::getline(f, s);

  aoc19::Computer c(s, true);
  c.initialize();

  Grid g;

  // Initial point is 0,0
  Point pos{0, 0};
  Direction dir = Direction::Up;
  aoc19::InputOutputs outputs;
  
  Point top_left{ 0, 0 };
  Point bottom_right{0, 0};

  c.initialize();
  c.set_input(0);

  while (true) {
    auto result = c.run(outputs);
    if (result) {
      break;
    }
    const auto color = outputs.front(); outputs.pop();
    assert (color == 1 || color == 0);

    Color nc;
    switch (color) {
      case 1:
        nc = Color::White;
        break;
      case 0:
        nc = Color::Black;
        break;
      default:
        throw std::runtime_error("Not a valid color");
    }

    auto ir = g.insert(std::pair(pos, Color::White));
    ir.first->second = nc;

    result = c.run(outputs);
    const auto turn = outputs.front(); outputs.pop();
    assert (turn == 1 || turn == 0);

    if (turn == 0) {
      dir = turn_left(dir);
    } else {
      dir = turn_right(dir);
    }

    const auto& w = Directions.find(dir);
    pos.first += w->second.first;
    pos.second += w->second.second;

    const auto paint = g.find(pos);
    c.set_input((paint == g.end() || paint->second == Color::Black) ? 0 : 1);

    top_left.first = std::min(top_left.first, pos.first);
    top_left.second = std::min(top_left.second, pos.second);

    bottom_right.first = std::max(bottom_right.first, pos.first);
    bottom_right.second = std::max(bottom_right.second, pos.second);

    if (result) {
      break;
    }
  }

  std::cout << "Part 1: " << g.size() << std::endl;
  DisplayGrid(g, top_left, bottom_right);

  return 0;
}

