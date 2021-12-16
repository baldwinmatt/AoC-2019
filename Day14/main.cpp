#include "aoc/helpers.h"
#include <map>
#include <queue>
#include <vector>
#include <cmath>

namespace {
  STRING_CONSTANT(ORE, "ORE");
  STRING_CONSTANT(FUEL, "FUEL");
  STRING_CONSTANT(ARROW, "=>");

  class Reagent {
  public:
    int64_t count;
    std::string name;

    Reagent()
      : count(0)
    { }

    Reagent(int64_t count, const std::string& name)
      : count(count)
      , name(name)
    { }

    Reagent(int64_t count, const std::string_view name)
      : count(count)
      , name(name.data(), name.size())
    { }

    friend std::ostream& operator<<(std::ostream& os, const Reagent& r) {
      os << r.count << " " << r.name;

      return os;
    }
  };

  class Reaction {
  public:
    Reagent output;
    std::vector<Reagent> inputs;

    Reaction() { }
    Reaction(const std::string& input) {
      std::stringstream ss(input);
      std::string tok;

      while (aoc::getline(ss, tok, " ,")) {
        if (ARROW.compare(std::string_view(tok.data(), tok.size())) == 0) {
          aoc::getline(ss, tok, ' ');
          output.count = std::stoll(tok);
          aoc::getline(ss, tok, ' ');
          output.name = tok;

          DEBUG_PRINT("Out: " << output);
        } else {
          int64_t count = std::stoll(tok);
          aoc::getline(ss, tok, " ,");
          inputs.emplace_back(count, tok);
        }
      }
    }
  };

  using Needs = std::queue<Reagent>;
  using Excess = std::map<std::string, int64_t, std::less<>>;
  using Reactions = std::map<std::string, Reaction, std::less<>>;

  const auto make_fuel = [](const Reactions& reactions, Excess& excess, bool make_ore, size_t& ore_needs) {
    Needs needs;
    needs.emplace(1, FUEL);

    while (!needs.empty()) {
      auto n = std::move(needs.front());
      needs.pop();

      if (make_ore && ORE.compare(n.name) == 0) {
        ore_needs += n.count;
        continue;
      }

      auto r = excess.find(n.name);
      if (r != excess.end()) {
        // we can satisfy with the excess we have
        if (n.count <= r->second) {
          r->second -= n.count;
          // and done
          continue;
        }
        n.count -= r->second;
        r->second = 0;
      }

      // make more of n
      DEBUG_PRINT("Need: " << n);

      if (!make_ore && ORE.compare(n.name) == 0) {
        return 0;
      }

      const auto re = reactions.find(n.name);
      assert(re != reactions.end());
      // figure out how many of these we need
      const int64_t needed = std::ceil(static_cast<double>(n.count) / static_cast<double>(re->second.output.count));

      // if we get more than we need, then shove it in excess
      const int64_t ex = (needed * re->second.output.count) - n.count;
      if (ex) {
        auto eit = excess.emplace(re->second.output.name, 0);
        eit.first->second += ex;
      }

      for (const auto& in : re->second.inputs) {
        needs.emplace(needed * in.count, in.name);
      }
    }

    return 1;
  };
};


int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  Reactions reactions;
  std::string line;
  while (aoc::getline(f, line)) {
    const auto out_start = line.find_last_of(' ');
    reactions.emplace(line.substr(out_start + 1), line);
  }
  f.close();


  size_t ore_needs = 0;
  {
    Excess excess;

    make_fuel(reactions, excess, true, ore_needs);
    aoc::print_result(1, ore_needs);
  }

  {
    size_t ore_needs = 0;
    Excess excess;
    excess.emplace(ORE, 1000000000000);
    size_t fuel = 0;
    while (true) {
      const auto f = make_fuel(reactions, excess, false, ore_needs);
      fuel += f;
      if (!f) {
        break;
      }
    }
    aoc::print_result(2, fuel);
  }

  return 0;
}

