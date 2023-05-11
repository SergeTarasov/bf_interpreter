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
    int bytecode_len;
    int instruction_pointer;
    int data_pointer;

} BrainFuck;


int max(int a, int b) {
    return a > b ? a : b;
}

void compile(BrainFuck *vm) {

    vm->instruction_pointer = 0;
    vm->data_pointer = 0;
    // word *bytecode = vm->bytecode;
    // todo
    // unsigned short stack[512]; // amount of nested loops basically
    // int loop_counter = 0;

    int val_incr_counter;  // count stacked '+'
    int val_decr_counter;  // count stacked '-'
    int dp_incr_counter;   // count stacked '>'
    int dp_decr_counter;   // count stacked '<'
    int ip;
    int bytecode_pointer = 0;

    while (vm->instruction_pointer < vm->code_len) {
        char instr = vm->code[vm->instruction_pointer];
        switch (instr) {
            case '>':
                dp_incr_counter = 0;

                ip = vm->instruction_pointer;
                while (vm->code[ip] == '>') {
                    dp_incr_counter++;
                    ip++;
                }
                vm->instruction_pointer = ip - 1;

                vm->bytecode[bytecode_pointer].instruction = OP_DP_INCR;
                vm->bytecode[bytecode_pointer].parameter = dp_incr_counter;
                break;
            case '<':
                dp_decr_counter = 0;

                ip = vm->instruction_pointer;
                while (vm->code[ip] == '<') {
                    dp_decr_counter++;
                    ip++;
                }
                vm->instruction_pointer = ip - 1;

                vm->bytecode[bytecode_pointer].instruction = OP_DP_DECR;
                vm->bytecode[bytecode_pointer].parameter = dp_decr_counter;
                break;
            case '+':
                val_incr_counter = 0;

                ip = vm->instruction_pointer;
                while (vm->code[ip] == '+') {
                    val_incr_counter++;
                    ip++;
                }
                vm->instruction_pointer = ip - 1;

                vm->bytecode[bytecode_pointer].instruction = OP_VAL_INCR;
                vm->bytecode[bytecode_pointer].parameter = val_incr_counter;
                break;
            case '-': {
                val_decr_counter = 0;

                ip = vm->instruction_pointer;
                while (vm->code[ip] == '-') {
                    val_decr_counter++;
                    ip++;
                }
                vm->instruction_pointer = ip - 1;

                vm->bytecode[bytecode_pointer].instruction = OP_VAL_DECR;
                vm->bytecode[bytecode_pointer].parameter = val_decr_counter;
            }
                break;
            case '.':
                vm->bytecode[bytecode_pointer].instruction = OP_PUTCHAR;
                break;
            case ',':
                vm->bytecode[bytecode_pointer].instruction = OP_GETCHAR;
                break;
            case '[': {
                vm->bytecode[bytecode_pointer].instruction = OP_LP_START;
                vm->bytecode[bytecode_pointer].parameter = 0;
            }
                break;
            case ']': {
                // find the opening bracket in the BYTE CODE and assign it current bytecode pointer
                ip = bytecode_pointer;
                int bracket_matching_number = 1;
                while (bracket_matching_number != 0) {
                    ip--;
                    word code = vm->bytecode[ip];
                    if (code.instruction == OP_LP_START) {
                        bracket_matching_number--;
                    } else if (code.instruction == OP_LP_END) {
                        bracket_matching_number++;
                    } else continue;
                }

                vm->bytecode[bytecode_pointer].instruction = OP_LP_END;
                vm->bytecode[bytecode_pointer].parameter = ip;
                // assign the opening instruction current address
                vm->bytecode[ip].parameter = bytecode_pointer;
            }
                break;
            default:
                // do not write comments to bytecode
                bytecode_pointer = max(bytecode_pointer - 1, 0);
                break;
        }
//        printf("%c, %d, %d\n", vm->code[vm->instruction_pointer], vm->bytecode[bytecode_pointer].instruction, vm->bytecode[bytecode_pointer].parameter);
        vm->instruction_pointer++;
        bytecode_pointer++;
    }
    vm->bytecode_len = bytecode_pointer;
}


void execute(BrainFuck *vm) {
    vm->instruction_pointer = 0;
    vm->data_pointer = 0;

    // printf("wtf: %d\n", vm->bytecode_len);
    while (vm->instruction_pointer < vm->bytecode_len) {
        word instr = vm->bytecode[vm->instruction_pointer];

        switch (vm->bytecode[vm->instruction_pointer].instruction) {
            case OP_DP_INCR:
                vm->data_pointer += instr.parameter;
                break;
            case OP_DP_DECR:
                vm->data_pointer -= instr.parameter;
                break;
            case OP_VAL_INCR:
                vm->memory[vm->data_pointer] += instr.parameter;
                break;
            case OP_VAL_DECR:
                vm->memory[vm->data_pointer] -= instr.parameter;
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
    memset(bytecode, 0, sizeof(word));

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
