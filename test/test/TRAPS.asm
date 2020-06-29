.orig x3000

	LEA R0,CHAR_IN 
	PUTS
	GETC
	AND R1,R1,#0
	ADD R1,R1,R0
	LEA R0, IN_CHAR
	PUTS
	AND R0,R0,#0
	ADD R0,R0,R1
	OUT
	IN
	AND R1,R1,#0
	ADD R1,R1,R0
	LEA R0, IN_CHAR
	PUTS
	AND R0,R0,#0
	ADD R0,R0,R1
	OUT
	HALT
CHAR_IN .STRINGZ "Ingrese carater\n:>"
IN_CHAR .STRINGZ "Carater ingresado es\n:>"
.end