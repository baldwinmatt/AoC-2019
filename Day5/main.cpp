#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <array>

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string s;
  aoc::getline(f, s);
  aoc19::Computer c(s);

  std::array<int, 2> in{ 1, 5 };
  for (int i = 0; i < 2; i++) {
    std::queue<int64_t> inputs;
    std::vector<int64_t> outputs;
    
    inputs.push(in[i]);

    c.initialize();
    const auto result = c.run(inputs, outputs);
    if (!result) {
        std::cerr << c << std::endl;

        std::cerr << "Error encountered, last opcode " <<
            c.get_last_op() << " at " << c.get_pc() << std::endl;
    }

    std::cout << "Part " << (i + 1) << ": " << outputs.back() << std::endl;
  }
  return 0;
}

