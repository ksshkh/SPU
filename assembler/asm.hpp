#ifndef ASM_HPP
#define ASM_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>

#include "../errors.hpp"
#include "../stack/stack.hpp"

enum Commands_code {
    CMD_DEFAULT = -2,
    CMD_HLT,
    CMD_DUMP,
    CMD_PUSH,
    CMD_POP,
    CMD_OUT,
    CMD_IN,
    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_SQRT,
    CMD_SIN,
    CMD_COS,
    CMD_JA,
    CMD_JAE,
    CMD_JB,
    CMD_JBE,
    CMD_JE,
    CMD_JNE,
    CMD_JMP
};

enum Registers {
    REG_DEFAULT,
    ax = 1,
    bx,
    cx,
    dx
};

const int argc_mask = 1 << 4;
const int reg_mask = 1 << 5;
const int mem_mask = 1 << 6;

const int VALUE_DEFAULT = -2;
const int NUM_OF_LABELS = 10;

struct Label {
    int address = 0;
    char* name = NULL;
};

struct Command {
    char *cmd = NULL;
    int cmd_code = 0;
    int argc = 0;
    int label = 0;
    Registers reg = REG_DEFAULT;
};

struct Assembler {
    const char* file_name_input = NULL;
    const char* file_name_print_txt = NULL;

    char* buf_input = NULL;
    int* buf_output = NULL;

    size_t size_file = 0;
    size_t n_cmd = 0;
    size_t n_words = 0;

    Command* cmds = NULL;
    Label* lbls = NULL;
};

int ProgramInput(Assembler* asmblr);

long int count_size_file(FILE* program);

int AsmCtor(Assembler* asmblr);

int count_num_of_words(const Assembler* asmblr);

int count_num_of_cmds(const Assembler* asmblr);

int CommandsParcing(Assembler* asmblr);

int Parcing(Assembler* asmblr);

void ArgumentsParcing(Assembler* asmblr, size_t i, char* argc);

int Output(Assembler* asmblr);

void label_insert(char* cmd, Assembler* asmblr, size_t j);

int label_find(char* cmd, Assembler* asmblr);

void AsmDump(Assembler* asmblr);

int AsmDtor(Assembler* asmblr);

#endif // ASM_HPP
