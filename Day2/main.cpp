#include "aoc/helpers.h"
#include "aoc/computer.h"
#include <vector>

int main(int argc, char **argv) {

    aoc::AutoTimer t;

    auto f = aoc::open_argv_1(argc, argv);
    std::string s;

    while (aoc::getline(f, s)) {
        aoc19::Computer c(s);
        {
            // Part 1
            c.initialize(12, 2);

            aoc19::InputOutputs inputs;
            aoc19::InputOutputs outputs;
            bool result = c.run(inputs, outputs);
            if (!result) {
                std::cerr << c << std::endl;
                std::cerr << "Error encountered, last opcode " <<
                    c.get_last_op() << " at " << c.get_pc() << std::endl;
            }
            std::cout << "Part 1: " << c.get(0) << std::endl;
        }
        // Part 2
        bool done = false;
        for (int64_t n = 0; !done && n < 100; n++) {
            for (int64_t v = 0; !done && v < 100; v ++) {
                c.initialize(n, v);
                aoc19::InputOutputs inputs;
                aoc19::InputOutputs outputs;
                bool result = c.run(inputs, outputs);
                if (!result) {
                    std::cerr << c << std::endl;
                    std::cerr << "Error encountered, last opcode " <<
                        c.get_last_op() << " at " << c.get_pc() << std::endl;
                }

                DEBUG(std::cout << c.get(0) << std::endl);
                
                done = c.get(0) == 19690720;
                if (done) {
                    DEBUG(std::cout << "Noun: " << n << ", Verb: " << v << std::endl);
                    std::cout << "Part 2: " << (n * 100 + v) << std::endl;
                }
            }
        }
    }

    return 0;
}
