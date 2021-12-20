#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <array>
#include <deque>
#include <map>
#include <chrono>
#include <thread>

namespace {

  using Point = std::pair<int, int>;

  enum class Direction {
    None = 0,
    North,
    South,
    West,
    East,
  };

  const std::vector<Direction> DirectionSet = {
    Direction::None,
    Direction::North,
    Direction::South,
    Direction::West,
    Direction::East,
  };

  const std::vector<Point> Directions = {
      {  0,  0 },
      {  0, -1 },
      {  0,  1 },
      { -1,  0 },
      {  1,  0 },
  };

  const auto Inverse = [](const Direction d) {
    switch (d) {
      case Direction::None:
        return d;
      case Direction::North:
        return Direction::South;
      case Direction::South:
        return Direction::North;
      case Direction::West:
        return Direction::East;
      case Direction::East:
        return Direction::West;
    }
  };
    
  enum class Tile {
    Unknown = -1,
    Wall = 0,
    Clear = 1,
    End = 2,
    Start = 3,
    Droid = 4,
  };

  class Grid {
  private:
    Point bottom_right_;
    Point top_left_;
    Point pos_;
    size_t end_cost_;
    std::map<Point, std::pair<Tile, size_t>> grid_;

  public:

    Grid()
      : bottom_right_(0, 0)
      , top_left_(0, 0)
      , pos_(0,0)
      , end_cost_(SIZE_T_MAX)
    {
      set(pos_, Tile::Start, 0);
    }

    Tile get(const Point& pt) const {
      if (pt == pos_) {
        return Tile::Droid;
      }

      const auto f = grid_.find(pt);
      if (f == grid_.end()) {
        return Tile::Unknown;
      }

      return f->second.first;
    }

    size_t get_end_cost() const {
      return end_cost_;
    }

    void set(const Point& pt, const Tile t, const size_t cost) {
      const auto r = grid_.emplace(pt, std::make_pair(t, cost));
      if (!r.second) {
        assert(r.first->second.first == t);
      }
    }

    void set_pos(const Point& pt) {
      pos_ = pt;
      top_left_.first = std::min(top_left_.first, pos_.first);
      top_left_.second = std::min(top_left_.second, pos_.second);
      bottom_right_.first = std::max(bottom_right_.first, pos_.first);
      bottom_right_.second = std::max(bottom_right_.second, pos_.second);
    }

    void do_move(Direction dir) {
      Point target = pos_;

      target.first += Directions[static_cast<int>(dir)].first;
      target.second += Directions[static_cast<int>(dir)].second;

      assert(get(target) != Tile::Wall);

      pos_ = target;
    }

    Point move(Direction dir) const {
      Point target = pos_;

      target.first += Directions[static_cast<int>(dir)].first;
      target.second += Directions[static_cast<int>(dir)].second;

      return target;
    }

    bool try_move(Direction dir) const {
      Point target = pos_;

      target.first += Directions[static_cast<int>(dir)].first;
      target.second += Directions[static_cast<int>(dir)].second;

      const auto g = get(target);
      switch (g) {
        case Tile::Unknown:
          return true;

        default:
          return false;
      }
    }

    friend std::ostream& operator<<(std::ostream& os, const Grid& grid) {
      const size_t height = grid.bottom_right_.second - grid.top_left_.second + 1;
      const size_t width = grid.bottom_right_.first - grid.top_left_.first + 1;

      for (size_t y = 0; y <= height; y++) {
        for (size_t x = 0; x <= width; x++) {
          const Point p{x + grid.top_left_.first, y + grid.top_left_.second};
          const auto c = grid.get(p);
          switch (c) {
            case Tile::Clear:
              os << " ";
              break;
            case Tile::Start:
              os << "S";
              break;
            case Tile::End:
              std::cout << "D";
              break;
            case Tile::Wall:
              os << "#";
              break;
            case Tile::Unknown:
              os << ".";
              break;
            case Tile::Droid:
              os << aoc::bold_on << "*" << aoc::bold_off;
              break;
          }
        }
        os << std::endl;
      }
      return os;
    }
  };

  class Movement {
  private:
    const Point pos_;
    const Direction reverse_;
    std::deque<Direction> next_;

  public:
    Movement(const Point& pt, Direction dir, const Grid& grid)
      : pos_(pt)
      , reverse_(Inverse(dir)) {
      for (const auto& d : DirectionSet) {
        if (d == Direction::None) {
          continue;
        }
        if (grid.try_move(d)) {
          next_.push_back(d);
        }
      }
    }

    bool has_next() const {
      return !next_.empty();
    }

    Direction next() {
      assert(has_next());

      auto d = next_.front();
      next_.pop_front();
      return d;
    }

    Direction reverse() const {
      return reverse_;
    }
  };

  using MoveList = std::deque<Movement>;
};

int main(int argc, char** argv) {

  aoc::AutoTimer _t;

  auto f = aoc::open_argv_1(argc, argv);
  std::string s;
  aoc::getline(f, s);
  f.close();

  const bool display =
#if !defined(INTERACTIVE)
    argc > 2 && argv[2][0] == '1';
#else
    true;
#endif

  aoc19::Computer c(s, true);
  aoc19::InputOutputs outputs;

  Grid grid;
  Point target_pos{0, 0};

  MoveList moves;
  c.initialize();

  moves.emplace_back(target_pos, Direction::None, grid);
  do {
#if !defined(INTERACTIVE)

    auto& move = moves.back();
    assert(move.has_next());
    Direction d = move.next();
    target_pos = grid.move(d);
    c.set_input(static_cast<int64_t>(d));
#endif

    const auto result = c.run(outputs);

    switch (result) {
      case aoc19::HaltCode::Halt:
        return 0;
      case aoc19::HaltCode::HasOutput:
        switch (outputs.front()) {
          case 0:
            grid.set(target_pos, Tile::Wall, 0);
            break;
          case 1:
            grid.set(target_pos, Tile::Clear, 0);
            grid.set_pos(target_pos);
            moves.emplace_back(target_pos, d, grid);  
            break;
          case 2:
            grid.set(target_pos, Tile::Clear, 0);
            grid.set_pos(target_pos);
            aoc::print_result(1, moves.size());
            return 0;
          default:
            break;
        }
        outputs.pop();
        if (display) {
          std::cout << aoc::cls;
          std::cout << grid << std::endl;
          std::cout << "Output: " << outputs.front() << std::endl;
#if !defined(INTERACTIVE)
          std::this_thread::sleep_for (std::chrono::milliseconds(10));
#endif
        }
        break;
      case aoc19::HaltCode::NeedsInput:
        {
#if !defined(INTERACTIVE)
          break;
#else
          std::cout << aoc::cls;
          std::cout << grid << std::endl;

          std::cout << "Input (1,2,3,4)> ";
          do {
            char in = std::getchar();
            if (in > '0' && in < '5') {
              const int move = in - '0';
              target_pos = grid.move(static_cast<Direction>(move));
              c.set_input(move);
              break;
            } else if (in == 'q') {
              return 0;
            }
          } while(true);
#endif
        }
        break;
      case aoc19::HaltCode::Error:
        std::cerr << "Unexpected error: " << std::endl;
        std::cerr << c << std::endl;
        return -1;
    }

    // Back-track
    {
      while (!moves.empty() && !moves.back().has_next()) {
        const auto d = moves.back().reverse();
        c.set_input(static_cast<int64_t>(d));
        aoc19::InputOutputs o;
        c.run(o);
        grid.do_move(d);
        moves.pop_back();
      }
    }
#if defined(INTERACTIVE)
  } while (true);
#else
  } while (!moves.empty());
#endif

  return 0;
}

