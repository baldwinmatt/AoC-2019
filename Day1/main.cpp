#include "aoc/helpers.h"

namespace {

class FuelCalculator {

public:

    FuelCalculator() {}

    int64_t calculate_fuel(int64_t input) {
        int64_t result = 0;

        int64_t fuel = input;
        do {
            fuel = calculate_step(fuel);
            result += fuel;
        } while (fuel > 0);

        return result;
    }

    int64_t calculate_step(int64_t input) {
        const int64_t step = input / 3;
        const int64_t result = step - 2;
        return result > 0 ? result : 0;
    }
};

};

int main(int argc, char **argv) {

    aoc::AutoTimer t;

    auto f = aoc::open_argv_1(argc, argv);

    FuelCalculator fc;

    int64_t total_fuel = 0;
    int64_t fuel_only = 0;
    std::string line;

    while (aoc::getline(f, line)) {
        int64_t v = std::stoll(line);
        total_fuel += fc.calculate_fuel(v);
        fuel_only += fc.calculate_step(v);
    }
    std::cout << "Part 1: " << fuel_only << std::endl;
    std::cout << "Part 2: " << total_fuel << std::endl;

    return 0;
}
