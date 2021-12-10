#include "aoc/helpers.h"
#include <vector>

namespace {

class  Computer
{
public:
    Computer(const std::string& program)
        : _last_opcode(0)
        , _last_pos(0) {
        aoc::parse_as_integers(program, ',', [&](const auto t) { _code.push_back(t); });
    }

    bool run(int64_t noun, int64_t verb, int64_t& output) {
        _code[1] = noun;
        _code[2] = verb;

        _last_pos = 0;
        while (_last_pos < _code.size()) {
            _last_opcode = _code.at(_last_pos);
            switch (_last_opcode) {
                case 1: // add
                {
                    const int64_t d1 = _code.at(_last_pos + 1);
                    const int64_t d2 = _code.at(_last_pos + 2);
                    const int64_t d3 = _code.at(_last_pos + 3);
                    assert(d3 >= 0 && d3 < static_cast<int64_t>(_code.size()));
                    _code[d3] = _code[d1] + _code[d2];
                    break;
                }
                case 2: // multiply
                {
                    const int64_t d1 = _code.at(_last_pos + 1);
                    const int64_t d2 = _code.at(_last_pos + 2);
                    const int64_t d3 = _code.at(_last_pos + 3);
                    assert(d3 >= 0 && d3 < static_cast<int64_t>(_code.size()));
                    _code[d3] = _code[d1] * _code[d2];
                    break;
                }
                case 99: // halt
                    output = _code[0];
                    return true;
                default: // invalid opcode
                    return false; 
            }
            _last_pos += 4;
        }

        return false;
    }

    int64_t _last_opcode;
    size_t _last_pos;

    std::vector<int64_t> _code;

    friend std::ostream& operator<<(std::ostream& os, const Computer& comp) {
        bool first = true;
        for (const auto c : comp._code) {
            if (!first) {
                os << ",";
            }
            os << c;
            first = false;
        }
        return os;
    }
};

};

int main(int argc, char **argv) {

    aoc::AutoTimer t;

    auto f = aoc::open_argv_1(argc, argv);
    std::string s;

    while (aoc::getline(f, s)) {
        {
            // Part 1
            Computer c(s);

            int64_t output = 0;
            bool result = c.run(12, 2, output);
            if (!result) {
                std::cerr << "Error encountered, last opcode " <<
                    c._last_opcode << " at " << c._last_pos << std::endl;
            }
            std::cout << "Part 1: " << output << std::endl;
        }
        // Part 2
        bool done = false;
        for (int64_t n = 0; !done && n < 100; n++) {
            for (int64_t v = 0; !done && v < 100; v ++) {
                Computer c(s);
                int64_t output = 0;
                bool result = c.run(n, v, output);
                if (!result) {
                    std::cerr << "Error encountered, last opcode " <<
                        c._last_opcode << " at " << c._last_pos << std::endl;
                }

                DEBUG(std::cout << output << std::endl);
                
                done = output == 19690720;
                if (done) {
                    DEBUG(std::cout << "Noun: " << n << ", Verb: " << v << std::endl);
                    std::cout << "Part 2: " << (n * 100 + v) << std::endl;
                }
            }
        }
    }

    return 0;
}
