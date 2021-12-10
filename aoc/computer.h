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
  99 - halt
*/

enum class ParameterMode {
    Position = 0,
    Immediate
};

class  Computer
{
public:
    Computer(const std::string& program)
        : _last_op(0)
        , _pc(0) {
        aoc::parse_as_integers(program, ',', [&](const auto t) { _init.push_back(t); });
        DEBUG(std::cout << "Memory Size: " << _init.size() << std::endl);
    }

    bool run(int64_t noun, int64_t verb, int64_t& output) {
        _memory = _init;

        _memory[1] = noun;
        _memory[2] = verb;

        std::vector<int64_t>outputs;

        const auto r = run(std::queue<int64_t>(), outputs);

        if (r) {
            output = outputs.back();
        }

        return r;
    }

    bool run(std::queue<int64_t> inputs, std::vector<int64_t>& outputs) {
        _memory = _init;
        _pc = 0;
        while (_pc < _memory.size()) {
            _last_op = _memory.at(_pc);

            const auto opcode = get_opcode();
            int advance_by = 0;

            DEBUG(std::cout << "PC: " << _pc << " Op " << _last_op << std::endl);

            switch (opcode) {
                case 1: // add
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_address(3);
                    DEBUG(std::cout << "Add: " << d1 << "," << d2 << " Store: " << d3 << std::endl);
                    assert(d3 >= 0 && d3 < static_cast<int64_t>(_memory.size()));
                    _memory[d3] = d1 + d2;
                    advance_by = 4;
                    break;
                }
                case 2: // multiply
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_address(3);
                    DEBUG(std::cout << "Mul: " << d1 << "," << d2 << " Store: " << d3 << std::endl);
                    assert(d3 >= 0 && d3 < static_cast<int64_t>(_memory.size()));
                    _memory[d3] = d1 * d2;
                    advance_by = 4;
                    break;
                }
                case 3: // input
                {
                    assert(!inputs.empty());
                    const auto value = inputs.front();
                    const auto address = get_address(1);
                    DEBUG(std::cout << "Input: " << value << " Store: " << address << std::endl);
                    assert(address >= 0 && address < static_cast<int64_t>(_memory.size()));
                    _memory[address] = value;
                    inputs.pop();
                    advance_by = 2;
                    break;
                }
                case 4: // output
                {
                    const auto value = get_parameter(1);
                    DEBUG(std::cout << "Output: " << value << std::endl);
                    outputs.push_back(value);
                    advance_by = 2;
                    break;
                }
                case 5: // Jump if true
                {
                    const auto value = get_parameter(1);
                    const auto new_pc = get_parameter(2);
                    DEBUG(std::cout << "Jump if True: " << value << "," << new_pc << std::endl);
                    if (!!value) {
                        _pc = new_pc;
                    }
                    else {
                        advance_by = 3;
                    }
                    break;
                }
                case 6: // Jump if false
                {
                    const auto value = get_parameter(1);
                    const auto new_pc = get_parameter(2);
                    DEBUG(std::cout << "Jump if False: " << value << "," << new_pc << std::endl);
                    if (!value) {
                        _pc = new_pc;
                    }
                    else {
                        advance_by = 3;
                    }
                    break;
                }
                case 7: // Less than
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_address(3);
                    DEBUG(std::cout << "LT: " << d1 << "," << d2 << " Store: " << d3 << std::endl);
                    assert(d3 >= 0 && d3 < static_cast<int64_t>(_memory.size()));
                    _memory[d3] = (d1 < d2);
                    advance_by = 4;
                    break;
                    
                }
                case 8: // Equal
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_address(3);
                    DEBUG(std::cout << "EQ: " << d1 << "," << d2 << " Store: " << d3 << std::endl);
                    assert(d3 >= 0 && d3 < static_cast<int64_t>(_memory.size()));
                    _memory[d3] = (d1 == d2);
                    advance_by = 4;
                    break;
                    
                }
                case 99: // halt
                    return true;
                default: // invalid opcode
                    return false; 
            }
            
            _pc += advance_by;
        }

        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const Computer& comp) {
        bool first = true;
        size_t p = 0;
        for (const auto c : comp._memory) {
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

protected:
    int64_t _last_op;
    size_t _pc;
    std::vector<int64_t> _memory;
    std::vector<int64_t> _init;

private:
    /// Opcode is a two-digit decimal number
    int64_t get_opcode() const {
        assert(_pc < _memory.size());
        return _memory[_pc] % 100;
    }

    /// Return the parameter mode for the parameter with the given index
    ParameterMode get_parameter_mode(int index) const {
        assert(_pc < _memory.size());
        if (1 == (_memory[_pc] / static_cast<int64_t>(std::pow(10, index + 1))) % 10) {
            return ParameterMode::Immediate;
        }
        return ParameterMode::Position;
    }

    /// Dereference memory at index based on the program counter
    int64_t get_address(int index) const {
        assert(_pc + index < _memory.size());
        return _memory[_pc + index];
    }

    /// Get the parameter value at index based on the program counter
    int64_t get_parameter(int index) const {

        const auto pos = _pc + index;

        switch (get_parameter_mode(index)) {
            case ParameterMode::Immediate:
                assert(pos < _memory.size());
                return _memory[pos];
            case ParameterMode::Position:
                assert(pos < _memory.size());
                const auto address = _memory[pos];
                assert(address < static_cast<int64_t>(_memory.size()));
                return _memory[address];
        }
        throw std::runtime_error("Invalid parameter mode");
    }
};

};
