#include <stdio.h>
#include <string.h>

enum opcodes {
    OP_ZERO,
    OP_DP_INCR,  // '>': increase data pointer
    OP_DP_DECR,  // '<': decrease data pointer
    OP_VAL_INCR, // '+': increase value in data cell
    OP_VAL_DECR, // '-': decrease value in data cell
    OP_PUTCHAR,  // '.': print a single character from data cell value
    OP_GETCHAR,  // ',': put a single character from input to data cell
    OP_LP_START, // '[': loop start, store the pointer to the (end of loop + 1)
    OP_LP_END    // ']': loop end, store the to the (start of loop + 1)
};


typedef struct {
    unsigned short instruction;
    unsigned short parameter;
} word;

typedef struct {
    char *code;
    word *bytecode;
    int *memory;
    int code_len;
    int instruction_pointer;
    int data_pointer;

} BrainFuck;

void compile(BrainFuck *vm) {

    vm->instruction_pointer = 0;
    vm->data_pointer = 0;
    // word *bytecode = vm->bytecode;
    // todo
    // unsigned short stack[512]; // amount of nested loops basically
    // int loop_counter = 0;

    while (vm->instruction_pointer < vm->code_len) {
        char instr = vm->code[vm->instruction_pointer];
        switch (instr) {
            case '>':
                vm->bytecode[vm->instruction_pointer].instruction = OP_DP_INCR;
                break;
            case '<':
                vm->bytecode[vm->instruction_pointer].instruction = OP_DP_DECR;
                break;
            case '+':
                vm->bytecode[vm->instruction_pointer].instruction = OP_VAL_INCR;
                break;
            case '-':
                vm->bytecode[vm->instruction_pointer].instruction = OP_VAL_DECR;
                break;
            case '.':
                vm->bytecode[vm->instruction_pointer].instruction = OP_PUTCHAR;
                break;
            case ',':
                vm->bytecode[vm->instruction_pointer].instruction = OP_GETCHAR;
                break;
            case '[': {
                unsigned short ip = vm->instruction_pointer;

                // find the closing bracket
                int bracket_matching_number = 1;
                while (bracket_matching_number != 0) {
                    ip++;
                    instr = vm->code[ip];
                    if (instr == '[') {
                        bracket_matching_number++;
                    } else if (instr == ']') {
                        bracket_matching_number--;
                    } else continue;
                }

                vm->bytecode[vm->instruction_pointer].instruction = OP_LP_START;
                vm->bytecode[vm->instruction_pointer].parameter = ip;
            }
                break;
            case ']': {
                unsigned short ip = vm->instruction_pointer;

                // find the opening bracket
                int bracket_matching_number = 1;
                while (bracket_matching_number != 0) {
                    ip--;
                    instr = vm->code[ip];
                    if (instr == '[') {
                        bracket_matching_number--;
                    } else if (instr == ']') {
                        bracket_matching_number++;
                    } else continue;
                }

                vm->bytecode[vm->instruction_pointer].instruction = OP_LP_END;
                vm->bytecode[vm->instruction_pointer].parameter = ip;
            }
                break;
            default:
                break;
        }
        vm->instruction_pointer++;
    }
}


void execute(BrainFuck *vm) {
    vm->instruction_pointer = 0;
    vm->data_pointer = 0;

    while (vm->instruction_pointer < vm->code_len) {
        word instr = vm->bytecode[vm->instruction_pointer];
        switch (instr.instruction) {
            case OP_DP_INCR:
                vm->data_pointer++;
                break;
            case OP_DP_DECR:
                vm->data_pointer--;
                break;
            case OP_VAL_INCR:
                vm->memory[vm->data_pointer]++;
                break;
            case OP_VAL_DECR:
                vm->memory[vm->data_pointer]--;
                break;
            case OP_PUTCHAR:
                putchar(vm->memory[vm->data_pointer]);
                break;
            case OP_GETCHAR:
                vm->memory[vm->data_pointer] = getchar();
                break;
            case OP_LP_START: {
                if (vm->memory[vm->data_pointer] == 0)
                    // go to loop end
                    vm->instruction_pointer = instr.parameter;
            }
                break;
            case OP_LP_END: {
                if (vm->memory[vm->data_pointer] != 0)
                    // go to loop start
                    vm->instruction_pointer = instr.parameter;
            }
                break;
            default:
                break;
        }
        vm->instruction_pointer++;
    }
}


int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Usage: ./program <filename>\n");
        return 1;
    }

    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char code[file_size];
    word bytecode[file_size];
    fread(code, 1, file_size, file);
    fread(code, 1, file_size, file);

    int memory[30000];
    memset(memory, 0, sizeof(memory));

    BrainFuck vm = {code, bytecode, memory, (int) file_size, 0, 0};

    compile(&vm);
    execute(&vm);

//    FILE *fp = fopen("filename.bin", "wb");
//    if (fp == NULL) {
//        printf("Failed to open file\n");
//        return 1;
//    }
//
//    fwrite(bytecode, sizeof(word), file_size, fp);
//    fclose(fp);

    return 0;
}
