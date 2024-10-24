#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <stdio.h>
#include <sys/stat.h>

#include "errors.hpp"

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
    ax,
    bx,
    cx,
    dx
};

const int ARGC_MASK = 1 << 4;
const int REG_MASK = 1 << 5;
const int MEM_MASK = 1 << 6;

const int VALUE_DEFAULT = -2;
const int NUM_OF_LABELS = 20;

long int count_size_file(FILE* program);

#endif
