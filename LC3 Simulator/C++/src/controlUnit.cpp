#include "../include/controlUnit.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

// const unsigned int HALT = 61477; //Trapx25 para finalizar el programa (F025)
const short int HALT = -4059; //Trapx25 para finalizar el programa (F025)
const short int END = 0;      //Constante para detectar la finalización del programa

controlUnit::controlUnit(short int *memory, unsigned int orig)
{
    Memory = memory;
    this->orig = orig;
    isBreakpoint("x", this->orig, breakpoints.get_x());
    systemClear();
    showRegisters();
    PC = orig;
    MDR = Memory[PC];
    simulate();
}
void controlUnit::simulate()
{
    while (MDR != HALT && MDR != END)
    {
        //TODO: Simular paso a paso
        FETCH();
    }
    if (MDR == HALT) //Una se salga del ciclo de trabajo identifica que causó la interrucion, si un HATL o el .END
    {
        instruction.opcode = MDR >> 12;
        DECODE();
    }
    else
    {
        showRegisters();
        cout << "Finaliza programa" << endl;
    }
}
void controlUnit::FETCH()
{
    MAR = PC;
    PC++; //Si la instrucción actual no implica cambiar el valor del PC
    isBreakpoint("PC", PC, breakpoints.get_PC());
    instruction.opcode = MDR >> 12;
    DECODE();
    //Segun el ciclo de instrucciones aqui iría EVALUATE_ADDRESS(), pero no es necesario porque se hace inderectamente en EVALUATE_ADDRESS() al obtener los offset y obtiene la localidad de memoria que se necesita para procesar la instrución dentro de DECODE() que asu vez está dentro de FECTCH() la anula de alguna forma, evitando hacer esa fase
    MDR = Memory[PC];
    IR = MDR;
    isBreakpoint("IR", IR, breakpoints.get_IR());
}
void controlUnit::DECODE()
{
    FETCH_OPERANDS(); //Optine los operandos de la instrucción
    //NOTA: La fase EXECUTE() y STORE_RESULT(), estan indirectamente implementadas en esta fase, al hacer las operaciones correspondiente a cada instrucción
    instruction.BEN = instruction.N & (CC == 'N') + instruction.P & (CC == 'P') + instruction.Z & (CC == 'Z'); //BEN<–IR[11] & N + IR[10] & Z + IR[9] & P
    switch (instruction.opcode)                                                                                //Evalua que instrucción se va a hacer
    {
    case 0: //BR
        if (instruction.BEN)
        {
            PC += instruction.PCoffset9;
            isBreakpoint("PC", PC, breakpoints.get_PC());
            cout << "BR" << endl;
        }
        break;
    case 1:                                                                                                   //ADD
        R[instruction.DR] = R[instruction.SR1] + instruction.is_imm5 ? instruction.imm5 : R[instruction.SR2]; //Si hay imediado a DR le lleva SR1 +imm5, sino le lleva SR1 +SR2
        isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        cout << "ADD" << endl;
        break;
    case 2: //LD
        MAR = PC + instruction.PCoffset9;
        MDR = Memory[MAR];
        R[instruction.DR] = MDR;
        isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        cout << "LD" << endl;
        break;
    case 3: //ST
        MAR = PC + instruction.PCoffset9;
        MDR = instruction.SR;
        Memory[MAR] = MDR;
        cout << "ST" << endl;
        break;
    case 4:        //JSR & JSRR
        R[7] = PC; //R7 <- PC
        isBreakpoint("R7", R[7], breakpoints.get_R(7));
        if (instruction.is_JSR) //JSR
        {
            PC = instruction.PCoffset11;
            isBreakpoint("PC", PC, breakpoints.get_PC());
            cout << "JSR" << endl;
        }
        else //JSRR
        {
            PC = R[instruction.BaseR];
            isBreakpoint("PC", PC, breakpoints.get_PC());
            cout << "JSRR" << endl;
        }
        break;
    case 5:                                                                                                   //AND
        R[instruction.DR] = R[instruction.SR1] & instruction.is_imm5 ? instruction.imm5 : R[instruction.SR2]; //Si hay imediado a DR le lleva SR1 and imm5, sino le lleva SR1 and SR2
        isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        cout << "AND" << endl;
        break;
    case 6: //LDR
        MAR = instruction.BaseR + instruction.offset6;
        MDR = Memory[MAR];
        R[instruction.DR] = MDR;
        isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        cout << "LDR" << endl;
        break;
    case 7: //STR
        MAR = instruction.BaseR + instruction.offset6;
        MDR = instruction.SR;
        Memory[MAR] = MDR;
        cout << "STR" << endl;
        break;
    case 8: //RTI
        break;
    case 9: //NOT
        R[instruction.DR] = ~(instruction.SR);
        isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        cout << "NOT" << endl;
        break;
    case 10: //LDI
        MAR = PC + instruction.PCoffset9;
        MDR = Memory[MAR];
        MAR = MDR;
        MDR = Memory[MAR];
        R[instruction.DR] = MDR;
        isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        cout << "LDI" << endl;
        break;
    case 11: //STI
        MAR = PC + instruction.PCoffset9;
        MDR = Memory[MAR];
        MAR = MDR;
        MDR = instruction.SR;
        Memory[MAR] = MDR;
        cout << "STI" << endl;
        break;
    case 12: //JMP
        PC = instruction.BaseR;
        isBreakpoint("PC", PC, breakpoints.get_PC());
        cout << "JMP" << endl;
        break;
    case 13: //RESERVED
        cout << "RESERVED" << endl;
        break;
    case 14: //LEA
        R[instruction.DR] = PC + instruction.PCoffset9;
        isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        cout << "LEA" << endl;
        break;
    default: //TRAP
        cout << "HALT" << endl;
        // MAR = instruction.trapvect8;
        // MDR = Memory[MAR];
        // PC = MDR;
        PC++;
        isBreakpoint("PC", PC, breakpoints.get_PC());
        MDR = Memory[PC];
        simulate();
        break;
    }
}

void controlUnit::FETCH_OPERANDS()
{
    //Se organiza segun el orden de aparición  de la tabla que hay en el libro "Introduction to computing systems" de la casa editorial Mc Graw Hill, pag 119
    instruction.DR = (MDR & 3584) >> 9;       //Hace una AND entre MDR y 0000 1110 0000 0000  y lo desplaza 9 posiciones hacia la derecha para obtener DR
    instruction.SR1 = (MDR & 448) >> 6;       //Hace una AND entre MDR y 0000 0001 1100 0000 y lo desplaza 6 posiciones hacia la derecha para obtener SR1
    instruction.is_imm5 = (MDR & 32) >> 5;    //Hace una AND entre MDR y 0000 0000 0010 0000 y lo desplaza 5 posiciones hacia la derecha para obtener la bandera para saber si hay imm5 (inmediato)
    instruction.SR2 = MDR & 7;                //Hace una AND entre MDR y 0000 0000 0000 0111, para obtener a SR2
    instruction.imm5 = MDR & 31;              //Hace una AND entre MDR y 0000 0000 0001 1111, para obtener a imm5
    instruction.N = (MDR & 2048) >> 11;       //Hace una AND entre MDR y 0000 1000 0000 0000 y lo desplaza 11 posiciones hacia la derecha para obtener N
    instruction.P = (MDR & 1024) >> 10;       //Hace una AND entre MDR y 0000 0100 0000 0000 y lo desplaza 10 posiciones hacia la derecha para obtener N
    instruction.Z = (MDR & 512) >> 9;         //Hace una AND entre MDR y 0000 0010 0000 0000 y lo desplaza 11 posiciones hacia la derecha para obtener N
    instruction.PCoffset9 = MDR & 511;        //Hace una AND entre MDR y 0000 0001 1111 1111, para obtener PCoffset9
    instruction.BaseR = (MDR & 448) >> 6;     //Hace una AND entre MDR y 0000 0001 1100 0000 y lo desplaza 6 posiciones hacia la derecha para obtener BaseR
    instruction.is_JSR = (MDR & 2048) >> 11;  //Hace una AND entre MDR y 0000 1000 0000 0000 y lo desplaza 11 posiciones hacia la derecha para saber si la instrucción es JSR
    instruction.is_JSRR = (MDR & 2048) >> 11; //Hace una AND entre MDR y 0000 1000 0000 0000 y lo desplaza 11 posiciones hacia la derecha para saber si la instrucción es JSRR
    instruction.PCoffset11 = MDR & 2047;      //Hace una AND entre MDR y 0000 0111 1111 1111, para obtener PCoffset11
    instruction.offset6 = MDR & 63;           //Hace una AND entre MDR y 0000 0000 0011 1111, para obtener offset6
    instruction.SR_NOT = (MDR & 448) >> 6;    //Hace una AND entre MDR y 0000 0001 1100 0000 y lo desplaza 6 posiciones hacia la derecha para obtener SR_NOT
    instruction.SR = (MDR & 3584) >> 9;       //Hace una AND entre MDR y 0000 1110 0000 0000  y lo desplaza 9 posiciones hacia la derecha para obtener SR
    instruction.trapvect8 = MDR & 255;        //Hace una AND entre MDR y 0000 0000 1111 1111, para obtener trapvec8
}

void controlUnit::setCC(short int result)
{
    if (result < 0)
    {
        CC = 'N';
    }
    else if (result > 0)
    {
        CC = 'P';
    }
    else
    {
        CC = 'Z';
    }
    isBreakpoint("CC", CC, breakpoints.get_CC());
}

void controlUnit::isBreakpoint(string nameRegister, short int currentValue, short int breakpointRegister)
{
    if (currentValue == breakpointRegister && breakpointRegister != 0)
    {
        systemClear();
        if (nameRegister == "CC")
        {
            // printf("Breakpoint found :> %s = %s\n",nameRegister.c_str() ,currentValue);
            printf("Breakpoint found :> %s = %c \n", nameRegister.c_str(), currentValue);
        }
        else
        {
            printf("Breakpoint found :> %s = 0x%x : %d \n", nameRegister.c_str(), currentValue, currentValue);
        }
        showRegisters();
    }
}
void controlUnit::showRegisters()
{
    cout << endl
         << "Registers:" << endl
         << endl;
    printf("PC :> 0x%x : %d\n", PC, PC);
    printf("x  :> 0x%x : %d\n", this->orig, this->orig);
    printf("IR :> 0x%x : %d\n", IR, IR);
    printf("CC :> 0x%x : %c\n", CC, CC);
    for (int i = 0; i < 8; i++)
    {
        printf("R%d :> 0x%x : %d\n", i, R[i], R[i]);
    }
    printf("\n\n");
    if (PC != 0)               //Solo pregunta si desea agregar otro archivo cuando ya haya empezado la simulación
    {
        if (isloadOtherFiles()) //Pregunta si desea cargar más archivos
            loadFiles();
    }
    systemPause();
    cout<<endl;
    systemClear();
}
void controlUnit::loadFiles()
{
    bool isLastInstructionHALT = (MDR == HALT);
    systemClear();
    file fileToSimulate;                 //Crea un objeto de tipo file, para carga el archivo a simular en la memoria
    cin.ignore();                        //Como en la siguiente función va a ver lectura de datos, entonces evito posibles problemas, ignorando el enter del último cin.get()
    fileToSimulate.selectPathType();     //Proceso de carga del archivo
    fileToSimulate.loadToMemory(Memory); //Carga el archivo en la memoria
    fileToSimulate.~file();              //Elimina el objeto porque ya no se utilizará, ya fue cargado en memoria
    breakpoints.~breakpoint();           //Elimina los anteriores breakpoints
    breakpoint breakpoints;              //Crea de nuevo un objeto breakpoint
    if (isloadOtherFiles())              //Pregunta si desea cargar más archivos
    {
        loadFiles();                     //Carga el otro archivo
        if(isLastInstructionHALT)
            simulate();
            //Recupera el .orig
    } 
}