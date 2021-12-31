#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <map>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>

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

  // Checking: A,B,B,A,C,A,C,A,C,B,
  // L,6,R,12,R,8,
  // R,8,R,12,L,12,
  // R,12,L,12,L,4,L,4,

  const auto IsFunctorValidLen = [](const auto& s) {
    const size_t max_size = 21;

    return s.size() <= max_size;
  };

  const auto IsReduced = [](const auto& s) {
    return s.find_first_not_of("ABC,") == std::string::npos;
  };

  const auto FeedAsciiInput = [](aoc19::Computer& c, const auto& s) {
    std::string_view sv(s.data(), s.size() - 1);
    for (const auto& ch : sv) {
      c.set_input(ch);
    }
    c.set_input('\n');
  };

  const auto ReplaceAll = [](const std::string& haystack, const std::string& needle, const std::string& replacement) {
    std::string out(haystack);
    const size_t nlen = needle.size();

    while (true) {
      const auto it = std::find_end(out.begin(), out.end(), needle.begin(), needle.end());
      if (it == out.end())
          return out;

      out.replace(it, it + nlen, replacement);
    }
  };

  class Function {
  public:
    std::string main;
    std::string func_a;
    std::string func_b;
    std::string func_c;

    bool valid() const {
      DEBUG_PRINT("Checking: " << main << " A: " << func_a << " B: " << func_b << " C: " << func_c);
      return IsFunctorValidLen(main) &&
        IsFunctorValidLen(func_a) &&
        IsFunctorValidLen(func_b) &&
        IsFunctorValidLen(func_c) &&
        IsReduced(main);
    }

    void feed_to_intcode(aoc19::Computer& c) const {
      // Feed the route to our computer, without trailing commas
      FeedAsciiInput(c, main);
      FeedAsciiInput(c, func_a);
      FeedAsciiInput(c, func_b);
      FeedAsciiInput(c, func_c);
    }
  };

  const auto ReduceRoute = [](const auto &in) {
    Function out;

    out.main = in;
    assert(!out.valid());

    while (!out.valid()) {
      auto start = in.begin();
      size_t len = out.func_a.empty() ? in.size() / 2 : out.func_a.size() - 1;
      for (; len > 1; len --) {
        const auto end = start + len;
        const auto r = std::search(end + 1, in.end(), start, end);
        if (r != in.end() && *(end - 1) == ',') {
          break;
        }
      }
      if (len == 1) {
        break;
      }
      out.func_a = std::move(in.substr(0, len));
      if (!IsFunctorValidLen(out.func_a)) {
        continue;
      }

      std::string main_b(ReplaceAll(in, out.func_a, "A,"));
      start = main_b.begin() + main_b.find_first_not_of("A,");
      len = main_b.size() / 2;
      for (; len > 1; len--) {
        const auto end = start + len;
        const auto r = std::search(end + 1, main_b.cend(), start, end);
        if (r == in.end() ||
            *(end - 1) != ',' ||
            std::find(start, end, 'A') != end) {
          continue;
        }

        out.func_b = std::string(start, end);
        if (!IsFunctorValidLen(out.func_b)) {
          continue;
        }

        std::string main_c(ReplaceAll(main_b, out.func_b, "B,"));
        const auto start_c = main_c.begin() + main_c.find_first_not_of("AB,");
        auto len_c = main_c.size() / 2;
        for (; len_c > 1; len_c --) {
          const auto end_c = start_c + len_c;
          auto r_c = std::search(end_c + 1, main_c.end(), start_c, end_c);

          static const std::string AB("AB");
          if (r_c == main_c.end() ||
              *(end_c - 1) != ',' ||
              find_first_of(start_c, end_c, AB.begin(), AB.end()) != end_c) {
            continue;
          }

          out.func_c = std::string(start_c, end_c);
          if (!IsFunctorValidLen(out.func_c)) {
            continue;
          }

          out.main = ReplaceAll(main_c, out.func_c, "C,");
          if (out.valid()) {
            return out;
          }
        }
      }
    }

    assert(out.valid());

    return out;
  };
};

int main(int argc, char** argv) {

  auto f = aoc::open_argv_1(argc, argv);
  std::string s;
  aoc::getline(f, s);
  f.close();

  aoc19::Computer c(s, true);
  
  Map map;
  c.initialize();

  const bool visualize = argc > 2 && argv[2][0] == '1';
  map.set_visualize(visualize);

  aoc19::HaltCode hc;
  do {
    aoc19::InputOutputs outputs;
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
  {
    aoc::AutoTimer t("Part 1");
    for (size_t y = 1; y < map.height() - 1; y++) {
      for (size_t x = 1; x < map.width() - 1; x++) {
        if (map.is_intersection(x, y)) {
          DEBUG_PRINT("Intersection: " << x << ", " << y);
          part1 += x * y;
        }
      }
    }
  }

  // reset the computer to initial state
  c.initialize();

  int64_t part2 = 0;

  {
    aoc::AutoTimer t("Part 2");

    c.set_memory(0, 2);

    const auto r = map.build_route();
    const auto rr = ReduceRoute(r);
    assert(rr.valid());

    rr.feed_to_intcode(c);

    c.set_input(visualize ? 'y' : 'n');
    c.set_input('\n');

    c.set_run_to_completion(true);
    aoc19::InputOutputs outputs;
    hc = c.run(outputs);

    while (!outputs.empty()) {
      part2 = outputs.front();
      outputs.pop();
    }
  }

  aoc::print_results(part1, part2);

  return 0;
}

