//
// Created by Nico Russo on 4/14/26.
//

#include "base.h"

using namespace omni;
using std::string;

enum OpKind {
    OP_LEFT = '<',
    OP_RIGHT = '>',
    OP_INC = '+',
    OP_DEC = '-',
    OP_OUT = '.',
    OP_IN = ',',
    OP_LOOP_S = '[',
    OP_LOOP_E = ']',
};

struct Op {
    OpKind kind;
    u32 operand;

    friend std::ostream& operator<<(std::ostream& os, const Op& v) {
        return os << (char)v.kind << " (" << v.operand << ")";
    }
};

class Machine {
private:
    std::array<u8, 2048> memory = {};
    u32 ptr = 0;


    std::vector<Op> program = {};

    std::ifstream* f;

    const char* valid = "<>+-.,[]";

    bool isBf (const char c) const {
        return strchr(valid, c) != nullptr;
    }

    void prepareProgram() {
        std::stack<u32> jumpStack;
        char ch = '\0';
        char prev = '\0';
        while (f->get(ch)) {
            if (isBf(ch)) {
                switch (ch) {
                    case '>':
                    case '<':
                    case '+':
                    case '-':
                    case '.':
                    case ',': {
                        if (ch != prev) {
                            Op op = {(OpKind)ch, 1 };
                            program.emplace_back(op);
                        } else {
                            program.back().operand += 1;
                        }
                        prev = ch;
                        break;
                    }
                    case '[': {
                        Op op = {(OpKind)ch, (u32)(program.size()) };
                        program.emplace_back(op);
                        jumpStack.push((u32)(program.size()-1));
                        prev = ch;
                        break;
                    }
                    case ']': {
                        Op op = {(OpKind)ch, jumpStack.top() };
                        program[jumpStack.top()].operand = program.size();
                        jumpStack.pop();
                        program.emplace_back(op);
                        prev = ch;
                        break;
                    }
                    default:
                        LOG_ERROR("Unknown command");
                }
            }
        }
        f->close();
    }

    void runProgram() {
        for (u32 prog_loc = 0; prog_loc < program.size(); ++prog_loc) {
            const Op op = program[prog_loc];

            switch (op.kind) {
                case OP_LEFT:
                    ptr = (ptr + memory.size() - (op.operand % memory.size())) % memory.size();
                    break;
                case OP_RIGHT:
                    ptr = (ptr + op.operand) % memory.size();
                    break;
                case OP_INC:
                    memory[ptr] += op.operand;
                    break;
                case OP_DEC:
                    memory[ptr] -= op.operand;
                    break;
                case OP_OUT: {
                    for (u32 i = 0; i < op.operand; ++i) {
                        print("{}", static_cast<char>(memory[ptr]));
                        std::cout.flush();
                    }
                    break;
                }
                case OP_IN: {
                    for (u32 i = 0; i < op.operand; ++i) {
                        const int ch = std::cin.get();
                        if (ch == EOF) {
                            memory[ptr] = 0;
                        } else {
                            memory[ptr] = (u8)ch;
                        }
                    }
                    break;
                }
                case OP_LOOP_S:
                    if (memory[ptr] == 0) {
                        prog_loc = op.operand;
                    }
                    break;
                case OP_LOOP_E:
                    if (memory[ptr] != 0) {
                        prog_loc = op.operand;
                    }
                    break;
            }
        }
    }

public:
    explicit Machine(std::ifstream* file) {
        f = file;
        f->seekg(0, std::ios::beg);
    }

    void run() {
        prepareProgram();
        runProgram();
    }
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        println("Usage: bf_cmp <brainfuck file>");
        return 1;
    }

    const string fname = argv[1];

    std::ifstream f(fname);
    if (f.fail()) {
        LOG_FATAL("failed to open file {}", fname);
        return 1;
    }

    Machine mach(&f);
    Timer t;
    mach.run();
    double ms = t.elapsed_ms();
    println("Time elapsed {} ms", ms);
    return 0;
}