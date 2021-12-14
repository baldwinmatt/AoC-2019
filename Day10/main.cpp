#include "aoc/helpers.h"
#include <set>
#include <cmath>

namespace {
  using Point = std::pair<int16_t, int16_t>;
  using AsteriodMap = std::set<Point>;

  template<typename T>
  T find_gcd(T a, T b) {
    if (b == 0) {
      return a;
    }
    T mod = a % b;
    return find_gcd(b, mod);
  }

  const auto pt_diff = [](const Point& lhs, const Point& rhs) {
      Point diff{lhs.first - rhs.first, lhs.second - rhs.second};

      return diff;
  };

  class Asteroid {
  public:
  
    Asteroid(const Point& pt, const Point& home)
      : pt(pt)
    {
      const auto diff = pt_diff(pt, home);

      angle = std::atan2f(static_cast<float>(diff.first), static_cast<float>(-diff.second));
      if (angle < 0.0f) {
        angle += 2.0f * M_PI;
      }
    }

    bool operator<(const Asteroid& rhs) const {
      return angle < rhs.angle;
    }

    const Point& pos() const {
      return pt;
    }

  private:
    const Point pt;
    float angle;
  };
  using VisibileSet = std::set<Asteroid>;

  const auto get_visible = [](const AsteriodMap& map, const Point& a) {
    AsteriodMap done;
    done.insert(a);

    VisibileSet vis;
    for (auto& t : map) {
      auto r = done.insert(t);
      if (!r.second) {
        continue;
      }
      Point diff = pt_diff(a, t);

      // calculate vector
      const auto gcd = std::abs(find_gcd(diff.first, diff.second));
      diff.first /= gcd;
      diff.second /= gcd;

      // See if we are blocked from the origin
      Point c = t;
      bool blocked = false;

      while (!blocked) {
        c.first += diff.first;
        c.second += diff.second;
        if (c == a) {
          break;
        }
        blocked = map.find(c) != map.end();
      }

      if (!blocked) {
        vis.insert({t, a});
      }
    }

    return vis;
  };

};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string s;
  size_t y = 0;
  AsteriodMap map;
  while (aoc::getline(f, s)) {
    for (size_t x = 0; x < s.size(); x++) {
      if (s[x] == '#') {
        map.emplace(x, y);
      }
    }
    y++;
  }
  f.close();

  size_t max_vis = 0;
  Point home;
  for (auto& a : map) {
    const auto v = get_visible(map, a);
    if (v.size() > max_vis) {
      home = a;
      max_vis = v.size();
    }
  }

  std::cout << "Part 1: " << max_vis << std::endl;

  size_t destroyed = 0;
  Point last{ 0, 0};
  
  while (destroyed < 200 && !map.empty()){
    // get all visible
    const auto vis = get_visible(map, home);

    for (const auto& a : vis) {
      last = a.pos();
      auto r = map.find(last);
      assert(r != map.end());
      map.erase(r);

      destroyed++;
      if (destroyed == 200 || map.empty()) {
        break;
      }
    }
  }

  std::cout << "Part 2: " << (last.first * 100 + last.second) << std::endl;

  return 0;
}

