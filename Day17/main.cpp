#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <array>
#include <map>
#include <vector>

namespace {

  enum class Direction {
    Up,
    Down,
    Left,
    Right
  };

  std::map<char, Direction> DirectionMap {
    { '^', Direction::Up },
    { 'v', Direction::Down },
    { '<', Direction::Left },
    { '>', Direction::Right },
  };

  using Point = std::pair<int, int>;
  using Robot = std::pair<Point, Direction>;

  class Map {
    size_t width_;
    size_t height_;
    std::vector<char> map_;
    Robot robot_;

  public:
    Map()
      : width_(0)
      , height_(1)
    { }

    void push_pixel(const char c) {
      switch (c) {
        case '#':
        case '.':
          map_.push_back(c);
          break;
        case '<':
        case '>':
        case '^':
        case 'v':
          {
            int x = map_.size();
            int y = 0;
            if (width_) {
              x = map_.size() % width_;
              y = map_.size() / width_;
            }
            const Point pos = std::make_pair(x, y);
            const auto d = DirectionMap.find(c);
            assert(d != DirectionMap.end());
            robot_ = std::make_pair(pos, d->second);
          }
          map_.push_back(c);
          break;
        case '\n':
          if (width_ == 0) {
            width_ = map_.size();
          }
          break;
        default:
          throw std::runtime_error(std::string("Invalid output: ") + c);
      }
      if (width_) {
        height_ = map_.size() / width_;
      }
    }

    size_t height() const { return height_; }
    size_t width() const { return width_; }

    char get(const Point& pt) {
      return get(pt.first, pt.second);
    }
    char get(size_t x, size_t y) const {
      assert(x >= 0 && x < width_);
      assert(y >= 0 && y < height_);
      const size_t pos = x + (y * width_);
      return map_[pos];
    }

    bool is_intersection(const Point& pt) const {
      return is_intersection(pt.first, pt.second);
    }

    bool is_intersection(size_t x, size_t y) const {
      assert(x >= 0 && x < width_);
      assert(y >= 0 && y < height_);

      if (x == 0 || x == width_ - 1) { return false; }
      else if (y == 0 || y == height_ -1) { return false; }

      return get(x, y) == '#' &&
        get(x - 1, y) == '#' &&
        get(x + 1, y) == '#' &&
        get(x, y - 1) == '#' &&
        get(x, y + 1) == '#';
    }

    friend std::ostream& operator<<(std::ostream& os, const Map& map) {
      for (size_t y = 0; y < map.height(); y++) {
        for (size_t x = 0; x < map.width(); x++) {
          os << (map.is_intersection(x, y) ? 'O' : map.get(x, y));
        }
        os << std::endl;
      }

      return os;
    }
  };
};

int main(int argc, char** argv) {

  auto f = aoc::open_argv_1(argc, argv);
  std::string s;
  aoc::getline(f, s);
  f.close();

  aoc19::Computer c(s, true);
  aoc19::InputOutputs outputs;
  
  Map map;
  c.initialize();
  aoc19::HaltCode hc;
  do {
    hc = c.run(outputs);
    switch (hc) {
      case aoc19::HaltCode::Halt:
        break;
      case aoc19::HaltCode::HasOutput:
        map.push_pixel(static_cast<char>(outputs.front()));
        outputs.pop();
        break;
      case aoc19::HaltCode::NeedsInput:
        {
          std::cout << "Input > ";
          int64_t in;
          std::cin >> in;
          c.set_input(in);
        }
        break;
      case aoc19::HaltCode::Error:
        std::cerr << "Unexpected error: " << std::endl;
        std::cerr << c << std::endl;
        return -1;
    }
  } while (hc != aoc19::HaltCode::Halt);

  DEBUG_PRINT(map);

  size_t part1 = 0;
  for (size_t y = 1; y < map.height() - 1; y++) {
    for (size_t x = 1; x < map.width() - 1; x++) {
      if (map.is_intersection(x, y)) {
        DEBUG_PRINT("Intersection: " << x << ", " << y);
        part1 += x * y;
      }
    }
  }

  aoc::print_result(1, part1);

  return 0;
}

