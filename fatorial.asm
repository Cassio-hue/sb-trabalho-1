SECTION TEXT
OUTRA_MACRO: MACRO
INPUT NEW_DATA
LOAD NEW_DATA
STORE NEW_DATA
ENDMACRO

MUL_N: MACRO 
OUTRA_MACRO
MUL N
STORE N
OUTRA_MACRO
ENDMACRO

INPUT N
LOAD N
FAT: SUB ONE
JMPZ FIM
STORE AUX
MUL_N 
LOAD AUX
JMP FAT
FIM: OUTPUT N
STOP
SECTION DATA
NEW_DATA: SPACE
AUX: SPACE
N: SPACE
ONE: CONST 1 