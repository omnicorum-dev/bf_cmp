//
// Created by Nico Russo on 4/14/26.
//

#include "base.h"

using namespace omni;
using std::string;

std::ofstream out;

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
    std::array<u8, 65536> memory = {};
    u32 ptr = 0;


    std::vector<Op> program = {};

    std::ifstream* f;
    std::ofstream* out;

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
                    *out << "ptr = (ptr + memory.size() - (op.operand % memory.size())) % memory.size();" << std::endl;
                    break;
                case OP_RIGHT:
                    ptr = (ptr + op.operand) % memory.size();
                    *out << "ptr = (ptr + op.operand) % memory.size();" << std::endl;
                    break;
                case OP_INC:
                    memory[ptr] += op.operand;
                    *out << "memory[ptr] += op.operand;" << std::endl;
                    break;
                case OP_DEC:
                    memory[ptr] -= op.operand;
                    *out << "memory[ptr] -= op.operand;" << std::endl;
                    break;
                case OP_OUT: {
                    *out << "for (u32 i = 0; i < op.operand; ++i) {" << std::endl;
                    *out << "    print("{}", static_cast<char>(memory[ptr]));" << std::endl;
                    *out << "    std::cout.flush();" << std::endl;
                    *out << "}" << std::endl;
                    break;
                }
                case OP_IN: {
                    *out << "for (u32 i = 0; i < op.operand; ++i) {" << std::endl;
                    *out << "    const int ch = std::cin.get();" << std::endl;
                    *out << "    if (ch == EOF) {" << std::endl;
                    *out << "        memory[ptr] = 0;" << std::endl;
                    *out << "    } else {" << std::endl;
                    *out << "        memory[ptr] = (u8)ch;" << std::endl;
                    *out << "    }" << std::endl;
                    *out << "}" << std::endl;
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
    explicit Machine(std::ifstream* file, std::ofstream* o) {
        f = file;
        out = o;
        f->seekg(0, std::ios::beg);
    }

    void run() {
        prepareProgram();
        runProgram();
    }
};

void header() {
    println(out, "#include <iostream>");
    println(out, "#include <vector>\n");
    out << "std::array<uint8_t, 65536> memory = {};\n";
    println(out, "uint32_t ptr = 0;\n");
    println(out, "int main() {");
    println(out, "std::cout << \"Hello, World!\" << std::endl;");
}

void footer() {
    println(out, "return 0;");
    println(out, "}\n");
}

int main(int argc, char* argv[]) {

    out.open("out.cpp");

    if (out.fail()) {
        LOG_ERROR("Can't open output file");
        return 1;
    }

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

    Machine mach(&f, &out);

    header();
    mach.run();
    footer();

    return 0;
}