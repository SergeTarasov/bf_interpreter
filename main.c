#include <stdio.h>
#include <string.h>


typedef struct {
    char *code;
    int *memory;
    int code_len;
    int instruction_pointer;
    int data_pointer;

} BrainFuck;


void execute(BrainFuck *vm) {
    while (vm->instruction_pointer < vm->code_len) {
        char instr = vm->code[vm->instruction_pointer];
        switch (instr) {
            case '>': {
                vm->data_pointer++;
            }
                break;
            case '<': {
                vm->data_pointer--;
            }
                break;
            case '+': {
                vm->memory[vm->data_pointer]++;
            }
                break;
            case '-': {
                vm->memory[vm->data_pointer]--;
            }
                break;
            case '.': {
                putchar(vm->memory[vm->data_pointer]);
            }
                break;
            case ',': {
                vm->memory[vm->data_pointer] = getchar();
            }
                break;
            case '[': {
                if (vm->memory[vm->data_pointer] == 0) {
                    int bracket_matching_number = 1;
                    while (bracket_matching_number != 0) {
                        vm->instruction_pointer++;
                        instr = vm->code[vm->instruction_pointer];
                        if (instr == '[') {
                            bracket_matching_number++;
                        } else if (instr == ']') {
                            bracket_matching_number--;
                        } else continue;
                    }
                }
            }
                break;
            case ']': {
                if (vm->memory[vm->data_pointer] != 0) {
                    int bracket_matching_number = 1;
                    while (bracket_matching_number != 0) {
                        vm->instruction_pointer--;
                        instr = vm->code[vm->instruction_pointer];
                        if (instr == '[') {
                            bracket_matching_number--;
                        } else if (instr == ']') {
                            bracket_matching_number++;
                        } else continue;
                    }
                }
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
    fread(code, 1, file_size, file);
    fread(code, 1, file_size, file);

    int memory[30000];
    memset(memory, 0, sizeof(memory));

    BrainFuck vm = {code, memory, (int) file_size, 0, 0};

    execute(&vm);
    return 0;
}
