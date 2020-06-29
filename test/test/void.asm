.orig x0000
    AND R0,R0,#0
    AND R1,R1,#0
    ADD R1,R1,#13
    LD  R2,lastestPos
    STR R0,R1,#0
    ADD R1,R1,#1
    ADD R3,R2,R1
    BRnp #-4
    AND R1,R1,#0
    AND R2,R2,#0
    AND R3,R3,#0
    lastestPos .FILL xFFFF
.end