#include "asm.hpp"

int AsmCtor(Assembler* asmblr) {

    MY_ASSERT(asmblr != NULL, PTR_ERROR);

    asmblr->file_name_input = "program.txt";
    asmblr->file_name_print_txt = "result.txt";

    CHECKED_ ProgramInput(asmblr);

    asmblr->n_cmd = count_num_of_cmds(asmblr);
    asmblr->n_words = count_num_of_words(asmblr);

    asmblr->lbls = (Label*)calloc(NUM_OF_LABELS, sizeof(Label));
    MY_ASSERT(asmblr->lbls != NULL, PTR_ERROR);

    for(int i = 0; i < NUM_OF_LABELS; i++) {
        asmblr->lbls[i].address = -1;
    }

    return code_error;
}

int ProgramInput(Assembler* asmblr) {

    MY_ASSERT(asmblr != NULL, PTR_ERROR);

    FILE* program = fopen(asmblr->file_name_input, "rb");
    MY_ASSERT(program != NULL, FILE_ERROR);

    asmblr->size_file = count_size_file(program);

    asmblr->buf_input = (char*)calloc(asmblr->size_file, sizeof(char));
    MY_ASSERT(asmblr->buf_input, PTR_ERROR);

    fread(asmblr->buf_input, sizeof(char), asmblr->size_file, program);

    MY_ASSERT(fclose(program) == 0,FILE_ERROR);

    return code_error;
}

// объединить

int count_num_of_words(const Assembler* asmblr) {

    int counter_words = 0;
    int counter_labels = 0;

    for(size_t i = 0; i < asmblr->size_file; i++) {

        if(isspace(asmblr->buf_input[i])) {
            if(isspace(asmblr->buf_input[i + 1])) {
                continue;
            }
            counter_words++;
        }
        if(asmblr->buf_input[i] == '+') {
                counter_words--;
        }
        else if(asmblr->buf_input[i] == ':') {
                counter_labels++;
        }
    }

    counter_words = counter_words - (counter_labels / 2);

    return counter_words + 1;
}

int count_num_of_cmds(const Assembler* asmblr) {

    int counter_cmds = 0;

    for(size_t i = 0; i < asmblr->size_file; i++) {

        if(asmblr->buf_input[i] == '\n') {
            if(asmblr->buf_input[i + 1] == '\n') {
                continue;
            }
            counter_cmds++;
        }
    }

    return counter_cmds + 1;
}

int Parcing(Assembler* asmblr) {

    MY_ASSERT(asmblr != NULL, PTR_ERROR);

    asmblr->cmds = (Command*) calloc(asmblr->n_cmd, sizeof(Command));
    MY_ASSERT(asmblr->cmds, PTR_ERROR);

    asmblr->cmds[0].cmd = asmblr->buf_input;
    int j = 1;

    for(size_t i = 0; i < asmblr->size_file; i++) { // optimize

        if(asmblr->buf_input[i] == '\n') {

            while(isspace(asmblr->buf_input[i + 1])) {      // ++i
                asmblr->buf_input[i] = '\0';
                i++;
            }

            asmblr->buf_input[i] = '\0';
            asmblr->cmds[j].cmd = (asmblr->buf_input + i + 1);
            j++;
        }
    }

    return code_error;
}

// make obrabotka argumentov

int CommandsParcing(Assembler* asmblr) {

    MY_ASSERT(asmblr != NULL, PTR_ERROR);

    asmblr->buf_output = (int*)calloc(asmblr->n_words, sizeof(int));
    MY_ASSERT(asmblr->buf_output != NULL, PTR_ERROR);

    for(int twice_comp = 0; twice_comp < 2; twice_comp++) {

        for(int i = 0, j = 0; i < asmblr->n_cmd; i++, j++) {

            char* cmd = asmblr->cmds[i].cmd;

            if(!strncmp(cmd, "hlt", 3)) {
                asmblr->cmds[i].cmd_code = CMD_HLT;
                asmblr->buf_output[j] = CMD_HLT;
            }

            else if(!strncmp(cmd, "dump", 4)) {
                asmblr->cmds[i].cmd_code = CMD_DUMP;
                asmblr->buf_output[j] = CMD_DUMP;
            }

            else if(!strncmp(cmd, "out", 3)) {
                asmblr->cmds[i].cmd_code = CMD_OUT;
                asmblr->buf_output[j] = CMD_OUT;
            }

            else if(!strncmp(cmd, "in", 2)) {
                asmblr->cmds[i].cmd_code = CMD_IN;
                asmblr->buf_output[j] = CMD_IN;
            }

            else if(!strncmp(cmd, "push", 4)) {
                char* argc = &(asmblr->cmds[i].cmd[5]);
                int flag = 0;

                asmblr->cmds[i].cmd_code = CMD_PUSH;
                ArgumentsParcing(asmblr, i, argc);
                asmblr->buf_output[j] = asmblr->cmds[i].cmd_code;
                j++;
                if((asmblr->cmds[i].cmd_code & ARGC_MASK) == ARGC_MASK) {
                    asmblr->buf_output[j] = asmblr->cmds[i].argc;
                    flag++;
                }
                if((asmblr->cmds[i].cmd_code & REG_MASK) == REG_MASK) {
                    asmblr->buf_output[j] = asmblr->cmds[i].reg;
                    flag++;
                }
                if(flag == 2) {
                    asmblr->buf_output[j] = asmblr->cmds[i].reg;
                    j++;
                    asmblr->buf_output[j] = asmblr->cmds[i].argc;
                }
            }

            else if(!strncmp(cmd, "pop", 3)) {
                char* argc = &(asmblr->cmds[i].cmd[4]);

                asmblr->cmds[i].cmd_code = CMD_POP;
                ArgumentsParcing(asmblr, i, argc);
                asmblr->buf_output[j] = asmblr->cmds[i].cmd_code;
                if((asmblr->cmds[i].cmd_code & REG_MASK) == REG_MASK) {
                    j++;
                    asmblr->buf_output[j] = asmblr->cmds[i].reg;
                }
                if((asmblr->cmds[i].cmd_code & ARGC_MASK) == ARGC_MASK) {
                    j++;
                    asmblr->buf_output[j] = asmblr->cmds[i].argc;
                }
            }

            else if(!strncmp(cmd, "add", 3)) {
                asmblr->cmds[i].cmd_code = CMD_ADD;
                asmblr->buf_output[j] = CMD_ADD;
            }

            else if(!strncmp(cmd, "sub", 3)) {
                asmblr->cmds[i].cmd_code = CMD_SUB;
                asmblr->buf_output[j] = CMD_SUB;
            }

            else if(!strncmp(cmd, "mul", 3)) {
                asmblr->cmds[i].cmd_code = CMD_MUL;
                asmblr->buf_output[j] = CMD_MUL;
            }

            else if(!strncmp(cmd, "div", 3)) {
                asmblr->cmds[i].cmd_code = CMD_DIV;
                asmblr->buf_output[j] = CMD_DIV;
            }

            else if(!strncmp(cmd, "sqrt", 4)) {
                asmblr->cmds[i].cmd_code = CMD_SQRT;
                asmblr->buf_output[j] = CMD_SQRT;
            }

            else if(!strncmp(cmd, "sin", 3)) {
                asmblr->cmds[i].cmd_code = CMD_SIN;
                asmblr->buf_output[j] = CMD_SIN;
            }

            else if(!strncmp(cmd, "cos", 3)) {
                asmblr->cmds[i].cmd_code = CMD_COS;
                asmblr->buf_output[j] = CMD_COS;
            }

            else if(!strncmp(cmd, "jae", 3)) {
                char* argc = &(asmblr->cmds[i].cmd[4]);

                asmblr->cmds[i].cmd_code = CMD_JAE;
                ArgumentsParcing(asmblr, i, argc);
                asmblr->buf_output[j] = asmblr->cmds[i].cmd_code;
                j++;
                asmblr->buf_output[j] = asmblr->cmds[i].label;
            }

            else if(!strncmp(cmd, "ja", 2)) {
                char* argc = &(asmblr->cmds[i].cmd[3]);

                asmblr->cmds[i].cmd_code = CMD_JA;
                ArgumentsParcing(asmblr, i, argc);
                asmblr->buf_output[j] = asmblr->cmds[i].cmd_code;
                j++;
                asmblr->buf_output[j] = asmblr->cmds[i].label;
            }

            else if(!strncmp(cmd, "jbe", 3)) {
                char* argc = &(asmblr->cmds[i].cmd[4]);

                asmblr->cmds[i].cmd_code = CMD_JBE;
                ArgumentsParcing(asmblr, i, argc);
                asmblr->buf_output[j] = asmblr->cmds[i].cmd_code;
                j++;
                asmblr->buf_output[j] = asmblr->cmds[i].label;
            }

            else if(!strncmp(cmd, "jb", 2)) {
                char* argc = &(asmblr->cmds[i].cmd[3]);

                asmblr->cmds[i].cmd_code = CMD_JB;
                ArgumentsParcing(asmblr, i, argc);
                asmblr->buf_output[j] = asmblr->cmds[i].cmd_code;
                j++;
                asmblr->buf_output[j] = asmblr->cmds[i].label;
            }

            else if(!strncmp(cmd, "je", 2)) {
                char* argc = &(asmblr->cmds[i].cmd[3]);

                asmblr->cmds[i].cmd_code = CMD_JE;
                ArgumentsParcing(asmblr, i, argc);
                asmblr->buf_output[j] = asmblr->cmds[i].cmd_code;
                j++;
                asmblr->buf_output[j] = asmblr->cmds[i].label;
            }

            else if(!strncmp(cmd, "jne", 3)) {
                char* argc = &(asmblr->cmds[i].cmd[4]);

                asmblr->cmds[i].cmd_code = CMD_JNE;
                ArgumentsParcing(asmblr, i, argc);
                asmblr->buf_output[j] = asmblr->cmds[i].cmd_code;
                j++;
                asmblr->buf_output[j] = asmblr->cmds[i].label;
            }

            else if(!strncmp(cmd, "jmp", 3)) {
                char* argc = &(asmblr->cmds[i].cmd[4]);

                asmblr->cmds[i].cmd_code = CMD_JMP;
                ArgumentsParcing(asmblr, i, argc);
                asmblr->buf_output[j] = asmblr->cmds[i].cmd_code;
                j++;
                asmblr->buf_output[j] = asmblr->cmds[i].label;
            }

            else if(strchr(cmd, ':') != NULL) {
                label_insert(cmd, asmblr, &j); //up
            }

            else {
                fprintf(stderr, "SNTXERR: '%s'\n", cmd);
            }
        }
    }

    return code_error;
}

void ArgumentsParcing(Assembler* asmblr, size_t i, char* argc) {

    char* close_sym = NULL;

    if(*argc == '[') {
        asmblr->cmds[i].cmd_code |= MEM_MASK;
        close_sym = strchr(argc, ']');

        if(close_sym != NULL) {
            *close_sym = '\0';
        }
        else {
            fprintf(stderr, "SNTXERR: '%s'\n", argc);
        }

        argc += 1;
    }

    int param = atoi(argc);

    if(*(argc + 1) == 'x') {
        asmblr->cmds[i].cmd_code |= REG_MASK;

        if(*argc == 'a') {
            asmblr->cmds[i].reg = ax;
        }
        else if(*argc == 'b') {
            asmblr->cmds[i].reg = bx;
        }
        else if(*argc == 'c') {
            asmblr->cmds[i].reg = cx;
        }
        else if(*argc == 'd') {
            asmblr->cmds[i].reg = dx;
        }

        if(strchr(argc, '+') != NULL) {
            argc += 5;
            param = atoi(argc);
            asmblr->cmds[i].argc = param;
            asmblr->cmds[i].cmd_code |= ARGC_MASK;
        }
    }

    else if(strchr(argc, ':') != NULL) {
        int addr = label_find(argc, asmblr);

        if(addr != -1) {
            asmblr->cmds[i].label = addr;
        }
        else {
            int err_addr = -1;
            char* label_name = strchr(argc, ' ') + 1;
            label_insert(label_name, asmblr, &err_addr);
        }
    }

    else if(*(argc - 3) == 'j' || *(argc - 4) == 'j') {
        asmblr->cmds[i].label = param;
    }

    else if(!strcmp((argc - 4), "pop")) {
        return;
    }

    else {
        asmblr->cmds[i].argc = param;
        asmblr->cmds[i].cmd_code |= ARGC_MASK;
    }

    if(close_sym != NULL) {
        *close_sym = ']';
    }
}

int Output(Assembler* asmblr) {

    MY_ASSERT(asmblr != NULL, PTR_ERROR);

    FILE* result = fopen(asmblr->file_name_print_txt, "w + b");
    MY_ASSERT(result != NULL, FILE_ERROR);

    fwrite(asmblr->buf_output, sizeof(int), asmblr->n_words, result);

    MY_ASSERT(fclose(result) == 0, FILE_ERROR);

    return code_error;
}

void label_insert(char* cmd, Assembler* asmblr, int* j) {

    size_t i = 0;
    while(i < NUM_OF_LABELS) {

        if(asmblr->lbls[i].name != NULL) {
            if(strcmp(cmd, asmblr->lbls[i].name) == 0) {
                break;
            }
        }
        else if(asmblr->lbls[i].address == -1) {
            break;
        }

        i++;
    }

    asmblr->lbls[i].address = *j;
    asmblr->lbls[i].name = cmd;
    (*j)--;
}

int label_find(char* cmd, Assembler* asmblr) {

    size_t i = 0;

    while(i < NUM_OF_LABELS) {

        if(asmblr->lbls[i].name != NULL) {
            if(strcmp(cmd, asmblr->lbls[i].name) == 0) {
                break;
            }
        }
        i++;
    }

    if(i > NUM_OF_LABELS) {
        return -1;
    }

    int addr = asmblr->lbls[i].address;

    return addr;
}

void AsmDump(Assembler* asmblr) {

    fprintf(stderr, "commands table:\n");

    for(size_t i = 0; i < asmblr->n_cmd; i++) {
        fprintf(stderr, "%s\n", asmblr->cmds[i].cmd);
        fprintf(stderr, "cmd code: %d argument: %d register: %d label: %d\n", asmblr->cmds[i].cmd_code, asmblr->cmds[i].argc, asmblr->cmds[i].reg, asmblr->cmds[i].label);
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "labels table:\n");

    for(size_t i = 0; i < NUM_OF_LABELS; i++) {
        fprintf(stderr, "address: %d name: %s", asmblr->lbls[i].address, asmblr->lbls[i].name);
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "output buffer:\n");

    for(size_t i = 0; i < asmblr->n_words; i++) {
        fprintf(stderr, "%d ", asmblr->buf_output[i]);
    }

    fprintf(stderr, "\n");
}

int AsmDtor(Assembler* asmblr) {

    MY_ASSERT(asmblr != NULL, PTR_ERROR);

    asmblr->file_name_input = NULL;
    asmblr->file_name_print_txt = NULL;

    free(asmblr->buf_input);
    asmblr->buf_input = NULL;
    free(asmblr->buf_output);
    asmblr->buf_output = NULL;

    asmblr->size_file = 0;
    asmblr->n_cmd = 0;
    asmblr->n_words = 0;

    free(asmblr->cmds);
    asmblr->cmds = NULL;
    free(asmblr->lbls);
    asmblr->lbls = NULL;

    return code_error;
}

