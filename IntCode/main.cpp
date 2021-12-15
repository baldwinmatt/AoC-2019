#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <array>

int main(int argc, char** argv) {

  auto f = aoc::open_argv_1(argc, argv);
  std::string s;
  aoc::getline(f, s);
  f.close();

  aoc19::Computer c(s, true);
  aoc19::InputOutputs outputs;
  
  c.initialize();
  do {
    const auto result = c.run(outputs);
    switch (result) {
      case aoc19::HaltCode::Halt:
        return 0;
      case aoc19::HaltCode::HasOutput:
        std::cout << "Output: " << outputs.front() << std::endl;
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
  } while (true);

  return 0;
}

