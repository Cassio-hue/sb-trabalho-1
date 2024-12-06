#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para mapeamento de instruções para opcodes
typedef struct {
    char instruction[10];
    char opcode[3];
} OpcodeMap;

// Mapeamento de instruções para opcodes
// TODO: Colocar os opcode do assembly inventado
OpcodeMap opcodeMap[] = {
    {"MOV", "01"},
    {"ADD", "02"},
    {"SUB", "03"},
    {"JMP", "04"},
    {"HLT", "FF"}
};

const int opcodeMapSize = sizeof(opcodeMap) / sizeof(opcodeMap[0]);

int main() {
    FILE *inputFile = fopen("program.asm", "r");
    FILE *outputFile = fopen("program.bin", "w");

    if (!inputFile || !outputFile) {
        perror("Erro ao abrir arquivos");
        return 1;
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}