#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <deque>

namespace {

  const auto run_single_amp = [](aoc19::Computer& c, const auto phase, const auto out) {
    aoc19::InputOutputs inputs;
    aoc19::InputOutputs outputs;

    inputs.push(phase);
    inputs.push(out);

    try {
      c.initialize();
      (void)c.run(inputs, outputs);

      assert(outputs.size() <= 1);
      return outputs.back();
    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;

      std::cerr << c << std::endl;

      std::cerr << "Error encountered, last opcode " <<
          c.get_last_op() << " at " << c.get_pc() << std::endl;
    }
    exit(-1);
  };

  const auto run_amp_chain = [](aoc19::Computer& c, const auto& phases) {
    int out = 0;

    for (const auto& p : phases) {
      out = run_single_amp(c, p, out);
    }

    return out;
  };

  const auto run_amp_feedback_chain = [](std::deque<aoc19::Computer>& amps, std::deque<int> phases) {
    int64_t output = 0;

    try {

      while (true) {
        aoc19::InputOutputs outputs;
        auto& amp = amps.front();

        if (!phases.empty()) {
          amp.initialize();
          amp.set_input(phases.front()); phases.pop_front();
        }

        amp.set_input(output);
        const auto r = amp.run(outputs);

        assert(outputs.size() <= 1);

        if (!outputs.empty()) {
          output = outputs.back();
        }

        if (r == aoc19::HaltCode::Halt) {
          return output;
        }

        // cycle amps
        amps.push_back(amps.front()); amps.pop_front();
      }
    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
    }

    exit(-1);
  };

};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string s;
  aoc::getline(f, s);

  {
    std::deque<int>phases = { 0, 1, 2, 3, 4};
    int64_t max_out = INT64_MIN;
    do {
      aoc19::Computer amp{ s, true };
      const int64_t out = run_amp_chain(amp, phases);

      DEBUG(std::cout << " Output: " << out << std::endl);
      max_out = std::max(max_out, out);
    } while (std::next_permutation(phases.begin(), phases.end()));

    std::cout << "Part 1: " << max_out << std::endl;
  }

  {
    std::deque<aoc19::Computer> amps { { s, true }, { s, true }, { s, true }, { s, true }, { s, true }, };
    std::deque<int>phases = { 5, 6, 7, 8, 9};
    int64_t max_out = INT64_MIN;
    do {
      const int64_t out = run_amp_feedback_chain(amps, phases);

      DEBUG(std::cout << " Output: " << out << std::endl);
      max_out = std::max(max_out, out);
    } while (std::next_permutation(phases.begin(), phases.end()));

    std::cout << "Part 2: " << max_out << std::endl;
  }

  return 0;
}

