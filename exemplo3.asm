SECTION TEXT
INPUT B ; Lê a base
INPUT H ; Lê a altura
LOAD B ; Carrega B no acumulador
MUL H ; ACC tem BxH
DIV DOIS ; ACC tem o resultado de (B+H)/2
STORE R ; Coloca soma (B+H)/2 em R
OUTPUT R ; Escreve o resultado
STOP ; Termina a execução do programa
SECTION DATA
B: SPACE ; Reserva espaço para a base
H: SPACE ; Reserva espaço para a altura
R: SPACE ; Reserva espaço para o resultado
DOIS: CONST 2 ; Reserva espaço para uma constante