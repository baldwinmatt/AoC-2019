#include "helpers.h"

#include <vector>
#include <queue>
#include <cmath>
#include <sstream>

#ifdef AOC_DEBUG
#define __DEBUG(x) do { \
    x; \
} while (0)
#else
#define __DEBUG(x)
#endif

#define __DEBUG_PRINT(x) do { __DEBUG(std::cout << x << std::endl); } while (0)


namespace aoc19 {

    using InputOutputs = std::queue<int64_t>;
    using Memory = std::vector<int64_t>;

    enum class HaltCode {
        HasOutput = 0,
        NeedsInput,
        Halt,
        Error,
    };
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
  1 - add(ADD)
  2 - multiply(MUL)
  3 - input(IN)
  4 - output(OUT)
  5 - Jump if true(JNZ)
  6 - Jump if false(JZ)
  7 - Less than(SLT)
  8 - Equal(SEQ)
  99 - halt
*/

class InvalidOpcode 
    : std::exception
{
public:
    InvalidOpcode(size_t pc, int64_t opcode) {
        std::stringstream ss;

        ss << "Invalid opcode [ pc: " << pc << ", op: " << opcode << " ]";

        error = ss.str();

        std::cerr << "InvalidOpcode: " << error;
    }

    const char * what() const throw () {
        return error.c_str();
    }

private:
    std::string error;
};

class  Computer
{
    enum class ParameterMode {
        Position = 0,
        Immediate,
        Relative
    };

public:
    Computer(const std::string& program)
        : Computer(program, false)
    {
    }

    Computer(const std::string& program, bool pause_on_output)
        : _last_op(0)
        , _pc(SIZE_MAX)
        , _relative_base(0)
        , _pause_on_output(pause_on_output) {
        aoc::parse_as_integers(program, ',', [&](const auto t) { _init.push_back(t); });
        __DEBUG_PRINT("Memory Size: " << _init.size());
    }

    void initialize(int64_t noun, int64_t verb) {
        initialize();

        store(1, noun);
        store(2, verb);
    }

    void initialize() {
        _memory = _init;
        _pc = 0;
        _relative_base = 0;
        while (!_inputs.empty()) {
            _inputs.pop();
        }
    }

    void set_memory(size_t address, int64_t value) {
        store(address, value);
    }

    void set_input(int64_t v) {
        _inputs.push(v);
    }

    void set_run_to_completion(bool v) {
        _pause_on_output = !v;
    }

    HaltCode run(const InputOutputs& inputs, InputOutputs& outputs) {
        _inputs = inputs;
        return run(outputs);
    }

    HaltCode run(InputOutputs& outputs) {
        if (!initialized()) {
            initialize();
        }

        while (true) {
            _last_op = get_address(0);

            const auto opcode = get_opcode();

            __DEBUG_PRINT("PC: " << _pc << " RB: " << _relative_base << " OP: " << _last_op);

            switch (opcode) {
                case 1: // add
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_output_address(3);
                    __DEBUG_PRINT("ADD: " << d1 << "," << d2 << " -> " << d3);
                    store(d3, d1 + d2);
                    _pc += 4;
                    break;
                }
                case 2: // multiply
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_output_address(3);
                    __DEBUG_PRINT("MUL: " << d1 << "," << d2 << " -> " << d3);
                    store(d3, d1 * d2);
                    _pc += 4;
                    break;
                }
                case 3: // input
                {
                    assert(!_inputs.empty());
                    if (_inputs.empty()) {
                        return HaltCode::NeedsInput;
                    }

                    const auto value = _inputs.front(); _inputs.pop();
                    const auto address = get_output_address(1);
                    __DEBUG_PRINT("IN: " << value << " -> " << address);
                    store(address, value);
                    _pc += 2;
                    break;
                }
                case 4: // output
                {
                    const auto value = get_parameter(1);
                    __DEBUG_PRINT("OUT: " << value);
                    outputs.push(value);
                    _pc += 2;
                    if (_pause_on_output) {
                        return HaltCode::HasOutput;
                    }
                    break;
                }
                case 5: // Jump if true
                {
                    const auto value = get_parameter(1);
                    const auto new_pc = get_parameter(2);
                    __DEBUG_PRINT("JNZ: " << value << "," << new_pc);
                    if (value) {
                        _pc = new_pc;
                    } else {
                        _pc += 3;
                    }
                    break;
                }
                case 6: // Jump if false
                {
                    const auto value = get_parameter(1);
                    const auto new_pc = get_parameter(2);
                    __DEBUG_PRINT("JZ: " << value << "," << new_pc);
                    if (!value) {
                        _pc = new_pc;
                    } else {
                        _pc += 3;
                    }
                    break;
                }
                case 7: // Less than
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_output_address(3);
                    __DEBUG_PRINT("SLT: " << d1 << "," << d2 << " -> " << d3);
                    store(d3, d1 < d2);
                    _pc += 4;
                    break;
                    
                }
                case 8: // Equal
                {
                    const auto d1 = get_parameter(1);
                    const auto d2 = get_parameter(2);
                    const auto d3 = get_output_address(3);
                    __DEBUG_PRINT("SEQ: " << d1 << "," << d2 << " -> " << d3);
                    store(d3, d1 == d2);
                    _pc += 4;
                    break;
                }
                case 9: // Adjust relative base
                {
                    const auto d1 = get_parameter(1);
                    __DEBUG_PRINT("ARB: " << d1);
                    _relative_base += d1;
                    _pc += 2;
                    break;
                }
                case 99: // halt
                    return HaltCode::Halt;
                default: // invalid opcode
                   throw InvalidOpcode(_pc, opcode);
            }
        }
        throw InvalidOpcode(_pc, _last_op);
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
        if (address >= _memory.size()) {
            return 0;
        }
        return _memory[address];
    }

    bool initialized() const {
        return _pc != SIZE_MAX;
    }

protected:
    int64_t _last_op;
    size_t _pc;
    size_t _relative_base;
    bool _pause_on_output;

    Memory _memory;
    Memory _init;
    InputOutputs _inputs;

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

    int64_t get_output_address(int index) {
        const auto val = get_address(index);

        switch (get_parameter_mode(index)) {
            case ParameterMode::Immediate:
                throw std::runtime_error("Immediate mode not supported for output address");
            case ParameterMode::Position:
                return val;
            case ParameterMode::Relative:
                return _relative_base + val;
        }
        throw std::runtime_error("Invalid parameter mode");
    }
};

};
