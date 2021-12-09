#include "aoc/helpers.h"
#include <array>

int main(int argc, char **argv) {
    aoc::AutoTimer t;

    auto in = aoc::open_argv_1(argc, argv);

    const auto IsSixDigits = [](const auto i) {
        return i >= 100000 && i < 1000000;
    };
    const auto IsIncreasing = [](auto i) {
        uint_fast8_t a, b, c, d, e, f;
        uint_fast8_t* digits[] = { &f, &e, &d, &c, &b, &a };

        for (int idx = 0; idx < 6; idx++) {
            *(digits[idx]) = i % 10;
            i /= 10;
        }

        return f >= e &&
            e >= d &&
            d >= c &&
            c >= b &&
            b >= a;
    };

    const auto ContainsPair = [](auto i) {
        uint_fast8_t a, b, c, d, e, f;
        uint_fast8_t* digits[] = { &f, &e, &d, &c, &b, &a };

        for (int idx = 0; idx < 6; idx++) {
            *(digits[idx]) = i % 10;
            i /= 10;
        }

        return f == e ||
            e == d ||
            d == c ||
            c == b ||
            b == a;
    };

    const auto ContainsExclusivePair = [](auto i) {
        uint_fast8_t a, b, c, d, e, f;
        uint_fast8_t* digits[] = { &f, &e, &d, &c, &b, &a };

        for (int idx = 0; idx < 6; idx++) {
            *(digits[idx]) = i % 10;
            i /= 10;
        }

        return ((a == b && b != c) ||
            (a != b && b == c && c != d) ||
            (b != c && c == d && c != e) ||
            (c != d && d == e && e != f) ||
            (d != e && e == f));
    };

    const auto NumberValid = [&](const auto i) {
        return IsSixDigits(i) &&
            IsIncreasing(i) &&
            ContainsPair(i);
    };

    const auto NumberValid2 = [&](const auto i) {
        return IsSixDigits(i) &&
            IsIncreasing(i) &&
            ContainsExclusivePair(i);
    };

    std::string input;
    std::getline(in, input);

    std::array<int64_t, 2>inputs{ 0 };
    size_t i = 0;
    aoc::parse_as_integers(input, '-', [&inputs, &i](const auto t) {
        if (i >= 2) return;
        inputs[i] = t;
        i++;
    });

    if (i < 2) {
        inputs[1] = inputs[0];
    }
    int64_t start = inputs[0];
    int64_t end = inputs[1];

    size_t count = 0;
    size_t count2 = 0;

    for (auto i = start; i <= end; i++) {
        const auto r = NumberValid(i);
        const auto r2 = NumberValid2(i);
        DEBUG(
            std::cout << i << ":" << (r ? "true" : "false") <<
                ":" << (r2 ? "true" : "false") << std::endl
        );
        count += (r ? 1 : 0);
        count2 += (r2 ? 1 : 0);
    }

    std::cout << "Part 1: " << count << std::endl;
    std::cout << "Part 2: " << count2 << std::endl;

    return 0;
}
