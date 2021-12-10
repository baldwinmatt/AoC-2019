#include "helpers.h"

#include <vector>
#include <queue>
#include <cmath>

namespace aoc19 {

/*
ABCDE
 1002

DE - two-digit opcode,      02 == opcode 2
 C - mode of 1st parameter,  0 == position mode
 B - mode of 2nd parameter,  1 == immediate mode
 A - mode of 3rd parameter,  0 == position mode,
                                  omitted due to being a leading zero
*/

/* Opcode:
  1 - add
  2 - multiply
  3 - input 
  4 - output
  5 - Jump if true
  6 - Jump if false
  7 - Less than
  8 - Equal
  99 - halt
*/

enum class ParameterMode {
    Position = 0,
    Immediate,
    Relative
};

class  Computer
{
public:
    Computer(const std::string& program)
        : _last_op(0)
        , _pc(0)
        , _relative_base(0) {
        aoc::parse_as_integers(program, ',', [&](const auto t) { _init.push_back(t); });
        DEBUG(std::cout << "Memory Size: " << _init.size() << std::endl);
    }

    void initialize(int64_t noun, int64_t verb) {
        _memory = _init;
        store(1, noun);
        store(2, verb);
    }

    void initialize() {
        _memory = _init;
    }

    bool run(std::queue<int64_t> inputs, std::vector<int64_t>& outputs) {
        if (_memory.empty()) {
            initialize();
        }
        _pc = 0;
        _relative_base = 0;

        while (1) {
            _last_op = get_address(0);

            const auto opcode = get_opcode();

            DEBUG(std::cout << "PC: " << _pc << " RB: " << _relative_base << " OP: " << _last_op << std::endl);

            switch (opcode) {
                case 1: // add
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_address(3);
                    DEBUG(std::cout << "ADD: " << d1 << "," << d2 << std::endl);
                    store(d3, d1 + d2);
                    _pc += 4;
                    break;
                }
                case 2: // multiply
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_address(3);
                    DEBUG(std::cout << "MUL: " << d1 << "," << d2 << std::endl);
                    store(d3, d1 * d2);
                    _pc += 4;
                    break;
                }
                case 3: // input
                {
                    assert(!inputs.empty());
                    const auto value = inputs.front();
                    const auto address = get_address(1);
                    DEBUG(std::cout << "IN: " << value << std::endl);
                    store(address, value);
                    inputs.pop();
                    _pc += 2;
                    break;
                }
                case 4: // output
                {
                    const auto value = get_parameter(1);
                    DEBUG(std::cout << "OUT: " << value << std::endl);
                    outputs.push_back(value);
                    _pc += 2;
                    break;
                }
                case 5: // Jump if true
                {
                    const auto value = get_parameter(1);
                    const auto new_pc = get_parameter(2);
                    DEBUG(std::cout << "JT: " << value << "," << new_pc << std::endl);
                    if (value) {
                        _pc = new_pc;
                    }
                    else {
                        _pc += 3;
                    }
                    break;
                }
                case 6: // Jump if false
                {
                    const auto value = get_parameter(1);
                    const auto new_pc = get_parameter(2);
                    DEBUG(std::cout << "JF: " << value << "," << new_pc << std::endl);
                    if (!value) {
                        _pc = new_pc;
                    }
                    else {
                        _pc += 3;
                    }
                    break;
                }
                case 7: // Less than
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_address(3);
                    DEBUG(std::cout << "LT: " << d1 << "," << d2 << std::endl);
                    store(d3, d1 < d2);
                    _pc += 4;
                    break;
                    
                }
                case 8: // Equal
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_address(3);
                    DEBUG(std::cout << "EQ: " << d1 << "," << d2 << std::endl);
                    store(d3, d1 == d2);
                    _pc += 4;
                    break;
                }
                case 9: // Adjust relative base
                {
                    const auto d1 = get_parameter(1);
                    const auto new_rb = _relative_base + d1;
                    DEBUG(std::cout << "REL: " << d1 << std::endl);
                    _relative_base = new_rb;
                    _pc += 2;
                    break;
                }
                case 99: // halt
                    return true;
                default: // invalid opcode
                    return false; 
            }
        }

        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const Computer& comp) {
        bool first = true;
        size_t p = 0;
        const auto& mem = comp._memory.empty() ? comp._init : comp._memory;
        for (const auto c : mem) {
            if (!first) {
                os << ",";
            }
            if (p == comp._pc) {
                os << aoc::bold_on << c << aoc::bold_off;
            } else {
                os << c;
            }
            first = false;
            p ++;
        }
        return os;
    }

    int64_t get_last_op() const {
        return _last_op;
    }

    size_t get_pc() const {
        return _pc;
    }

    int64_t get(size_t address) const {
        if (address > _memory.size()) {
            return 0;
        }
        return _memory[address];
    }

protected:
    int64_t _last_op;
    size_t _pc;
    size_t _relative_base;
    std::vector<int64_t> _memory;
    std::vector<int64_t> _init;

private:

    /// Opcode is a two-digit decimal number
    int64_t get_opcode() {
        return get(_pc) % 100;
    }

    /// Return the parameter mode for the parameter with the given index
    ParameterMode get_parameter_mode(int index) {
        const int mode = (get(_pc) / static_cast<int64_t>(std::pow(10, index + 1))) % 10;
        switch (mode) {
            case 0:
                return ParameterMode::Position;
            case 1:
                return ParameterMode::Immediate;
            case 2:
                return ParameterMode::Relative;
        }
        throw std::runtime_error("Invalid parameter mode");
    }

    void store(size_t address, int64_t val) {
        if (address >= _memory.size()) {
            _memory.resize(address + 1, 0);
            DEBUG(std::cout << "Memory Size: " << _memory.size() << std::endl);
        }
        _memory[address] = val;
    }

    /// Dereference memory at index based on the program counter
    int64_t get_address(int index) {
        return get_relative_address(_pc, index);
    }

    int64_t get_relative_address(size_t base, size_t offset) {
        return get(base + offset);
    }

    /// Get the parameter value at index based on the program counter
    int64_t get_parameter(int index) {

        const auto val = get_address(index);

        switch (get_parameter_mode(index)) {
            case ParameterMode::Immediate:
                return val;
            case ParameterMode::Position:
                return get_relative_address(0, val);
            case ParameterMode::Relative:
                return get_relative_address(_relative_base, val);
        }
        throw std::runtime_error("Invalid parameter mode");
    }
};

};
