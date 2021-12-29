#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <map>
#include <vector>
#include <chrono>
#include <thread>

namespace {

  enum class Direction {
    Up,
    Right,
    Down,
    Left,
  };

  const auto GetDirection = [](const char c) {
    switch (c) {
      case '^': return Direction::Up;
      case '>': return Direction::Right;
      case 'v': return Direction::Down;
      case '<': return Direction::Left;
    }
    throw std::runtime_error("Invalid robot");
  };

  const auto GetCursor = [](const Direction c) {
    switch (c) {
      case Direction::Up: return '^';
      case Direction::Right: return '>';
      case Direction::Down: return 'v';
      case Direction::Left: return '<';
    }
    throw std::runtime_error("Invalid robot");
  };

  using Point = std::pair<int, int>;
  using Robot = std::pair<Point, Direction>;

  Point& operator+=(Point& lhs, const Point& rhs) {
    lhs.first += rhs.first;
    lhs.second += rhs.second;

    return lhs;
  }

  Point operator+(const Point& lhs, const Point& rhs) {
    return {lhs.first + rhs.first, lhs.second + rhs.second };
  }

  Point operator-(const Point& lhs, const Point& rhs) {
    return {lhs.first - rhs.first, lhs.second - rhs.second };
  }

  const std::vector<Point> Directions = {
      {  0, -1 },
      {  1,  0 },
      {  0,  1 },
      { -1,  0 },
  };

  class Map {
    size_t width_;
    size_t height_;
    std::vector<char> map_;
    Robot robot_;
    bool visualize_;

  public:
    Map()
      : width_(0)
      , height_(1)
      , visualize_(false)
    { }

    void set_visualize(bool v) {
      visualize_ = v;
    }

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
            const auto d = GetDirection(c);
            robot_ = std::make_pair(pos, d);
          }
          map_.push_back('#');
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

    char get(const Point& pt) const {
      const size_t x =  pt.first;
      const size_t y =  pt.second;
      if (x >= width_ ||  y >= height_) { return 'X'; }
      if (pt == robot_.first) {
        return GetCursor(robot_.second);
      }
      const size_t pos = x + (y * width_);
      return map_[pos];
    }
    char get(int x, int y) const {
      return get(std::make_pair(x, y));
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

    std::string build_route() {
      std::stringstream route;

      while (true) {
        auto dir = Directions[static_cast<int>(robot_.second)];

        size_t steps = 0;
        while (get(robot_.first + dir) == '#') {
          robot_.first += dir;
          if (visualize_) {
            std::cout << aoc::cls << *this;
            std::this_thread::sleep_for (std::chrono::milliseconds(100));
          }
          steps++;
        }
        if (steps) {
          route << steps << ",";
        }

        // try and go right
        int turn = (static_cast<int>(robot_.second) + 1) & 3;
        if (get(robot_.first + Directions[turn]) == '#') {
          robot_.second = static_cast<Direction>(turn);
          if (visualize_) {
            std::cout << aoc::cls << *this;
            std::this_thread::sleep_for (std::chrono::milliseconds(100));
          }
          route << "R,";
          continue;
        }

        // try and go left
        turn = (static_cast<int>(robot_.second) + 3) & 3;
        if (get(robot_.first + Directions[turn]) == '#') {
          robot_.second = static_cast<Direction>(turn);
          if (visualize_) {
            std::cout << aoc::cls << *this;
            std::this_thread::sleep_for (std::chrono::milliseconds(10));
          }
          route << "L,";
          continue;
        }

        break;
      }

      return route.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Map& map) {
      for (size_t y = 0; y < map.height(); y++) {
        for (size_t x = 0; x < map.width(); x++) {
          const auto c = map.get(x, y);
          switch (c) {
            case '<':
            case '>':
            case '^':
            case 'v':
              os << aoc::bold_on << c << aoc::bold_off;
              break;
            default:
              os << c;
          }
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

  map.set_visualize(argc > 2 && argv[2][0] == '1');

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

  const auto r = map.build_route();

  aoc::print_result(1, part1);
  aoc::print_result(2, r);

  return 0;
}

