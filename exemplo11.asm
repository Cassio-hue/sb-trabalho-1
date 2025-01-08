

;que loucura
            SECTION TEXT
COPY ZERO, OLDER
COPY ONE, OLD ;loucura
INPUT LIMIT
OUTPUT OLD
FRONT: 
    LOAD OLDER
    ADD OLD
                     STORE NEW
    SUB                               LIMIT
    JMPP             FINAL
    OUTPUT NEW
    COPY OLD       ,                      OLDER
    COPY NEW, OLD
    JMP FRONT
FINAL: 



    OUTPUT LIMIT
    STOP
    SECTION DATA
        ZERO: CONST 0
                    ONE: 
                    CONST 1
                            NEGATIVO: CONST -0x10
        OLDER: SPACE
                        OLD: SPACE
                                    NEW: SPACE
LIMIT: SPACE