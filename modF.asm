MOD_B: BEGIN

PUBLIC MAIOR
PUBLIC MENOR
PUBLIC COMP
N1: EXTERN
N2: EXTERN
N3: EXTERN
CONTINUE: EXTERN

COMP: LOAD N1
SUB N2
JMPP N1_MAIOR_N2
JMPN N2_MAIOR_N1

N2_MAIOR_N1: COPY N1, MENOR
    COPY N2, MAIOR
    LOAD N2
    SUB N3
    JMPN N3_MAIOR
    JMPP N3_MENOR

N1_MAIOR_N2: COPY N2, MENOR
    COPY N1, MAIOR
    LOAD N1
    SUB N3
    JMPN N3_MAIOR
    JMPP N3_MENOR

N3_MAIOR: COPY N3, MAIOR
    JMP PRINT

N3_MENOR: LOAD MENOR
    SUB N3
    JMPN PRINT
    COPY N3, MENOR

PRINT: OUTPUT MAIOR
    OUTPUT MENOR

JMP CONTINUE

MAIOR: SPACE
MENOR: SPACE

END