#include <iostream>
#define DISABLE_TRACE
#include "base.h"
#include <fstream>

using namespace omni;
using std::string;

enum OpCode {
    PTR_L,
    PTR_R,
    INC,
    DEC,
    PRNT,
    INPT,
    LOOP_S,
    LOOP_E,
    COMMENT,
};

OpCode charToOpCode (const char c) {
    switch (c) {
        case '<':
            return PTR_L;
        case '>':
            return PTR_R;
        case '+':
            return INC;
        case '-':
            return DEC;
        case '.':
            return PRNT;
        case ',':
            return INPT;
        case '[':
            return LOOP_S;
        case ']':
            return LOOP_E;
        default:
            return COMMENT;
    }
}

class Machine {
private:

    std::array<u8, 2048> memory = {};
    u32 ptr = 0;
    u32 code_loc = 0;
    std::vector<u32> jumpTable = {};
    std::vector<char> code = {};

    std::ifstream* f;

    void processCharacter(const char c) {
        const OpCode opcode = charToOpCode (c);
        switch (opcode) {
            case PTR_L:
                LOG_TRACE("<");
                if (ptr == 0) {
                    ptr = memory.size() - 1;
                } else {
                    ptr -= 1;
                }
                break;
            case PTR_R:
                LOG_TRACE(">");
                if (ptr == memory.size() - 1) {
                    ptr = 0;
                } else {
                    ptr += 1;
                }
                break;
            case INC:
                LOG_TRACE("+");
                memory[ptr] += 1;
                break;
            case DEC:
                LOG_TRACE("-");
                memory[ptr] -= 1;
                break;
            case PRNT:
                print("{}", static_cast<char>(memory[ptr]));
                std::cout.flush();
                break;
            case INPT: {
                const int ch = std::cin.get();
                if (ch == EOF) {
                    memory[ptr] = 0;
                } else {
                    memory[ptr] = static_cast<u8>(ch);
                }
            }
                break;
            case LOOP_S:
                if (memory[ptr] == 0) {
                    // jump to matching ]
                    u32 bracket_level = 1;
                    code_loc += 1;
                    for (; code_loc < code.size(); ++code_loc) {
                        if (code[code_loc] == '[') {
                            ++bracket_level;
                        } else if (code[code_loc] == ']') {
                            --bracket_level;
                        }

                        if (bracket_level == 0) {
                            break;
                        }
                    }
                } else {
                    jumpTable.push_back(code_loc);
                }
                break;
            case LOOP_E:
                if (memory[ptr] != 0) {
                    code_loc = jumpTable.back();
                } else {
                    jumpTable.pop_back();
                }
                break;
            default:
                return;
        }
    }

    void loadProgram() {
        char ch;
        while (f->get(ch)) {
            if (ch == '<' || ch == '>' || ch == '+' || ch == '-' || ch == '.' || ch == ',' || ch == '[' || ch == ']') {
                code.push_back(ch);
            }
        }
        f->close();
    }

    void runProgram() {
        for (; code_loc < code.size(); ++code_loc) {
            //print("{}", code[code_loc]);
            processCharacter(code[code_loc]);
        }
    }

public:

    explicit Machine(std::ifstream* file) {
        f = file;
        f->seekg(0, std::ios::beg);
    }

    void run() {
        loadProgram();
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

    Machine machine(&f);

    Timer t;

    machine.run();

    double ms = t.elapsed_ms();
    println("Time elapsed {} ms", ms);

    return 0;
}