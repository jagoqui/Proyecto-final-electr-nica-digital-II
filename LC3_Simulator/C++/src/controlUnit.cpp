#include "../include/controlUnit.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

const unsigned short int MAX_VALUE = 65535; //Valor máximo que puede tomar en decimal un número de 16 bits, se utilizará para hacer complemento a2, ya que short int es de 32 bits y llena los bits restantes con F, volviendo siempore el número negativo
const short int HALT = -4059;               //Trapx25 para finalizar el programa (F025)
const short int HALTING_END = 1172;         //Posicion de memoria donde termina el halting (x0494)
const short int OS_KBSR = -512;             //Key board status register (xFE00)
const short int OS_KBDR = -510;             //Key board data register (xFE02)
const short int OS_DSR = -508;              //Display status register (xFE04)
const short int OS_DDR = -506;              //Display data register (xFE06)
short int IR_BR_JMP_JSR;                    //IR para la instrucciones BR, JMP y JSR
int pos;                                    //Posición de la memoria

controlUnit::controlUnit(short int *memory, unsigned int orig, string fileName)
{
    Memory = memory;
    this->orig = orig;
    this->fileName = fileName;
    initialize(); //Incializa el PC y el MDR
    simulate();   //Inicia simulación
}

void controlUnit::initialize()
{
    PC = orig;
    if (breakpoints.get_PC() != NULL)
        isBreakpoint("PC", PC, *breakpoints.get_PC());
    MDR = Memory[PC];
    showRegisters(false);
    systemClear();
}

void controlUnit::showRegisters(bool isBreakpoint)
{
    short int aux_PC = PC;

    if (isBreakpoint)
    {
        aux_PC++;
        if (breakpoints.get_PC() != NULL)
        {
            if (*breakpoints.get_PC() == PC)
                aux_PC--;
        }
    }
    bool is_BR_JMP_JSR_TRAP = (instruction.opcode == BR) || (instruction.opcode == JMP) || (instruction.opcode == JSR_JSRR) || (instruction.opcode == TRAP);
    if (PC == orig)
    {
        systemClear();
        cout << "START....." << endl
             << endl;
    }
    cout << endl
         << "Registers:" << endl
         << endl;
    printf("PC :> 0x%04X : %d\n", aux_PC & MAX_VALUE, aux_PC);
    printf("IR :> 0x%04X : %d\n", (is_BR_JMP_JSR_TRAP ? IR_BR_JMP_JSR : IR) & MAX_VALUE, instruction.opcode == BR ? IR_BR_JMP_JSR : IR);
    printf("CC :> 0x%04X : %c\n", CC & MAX_VALUE, CC);
    for (int i = 0; i < 8; i++)
    {
        printf("R%d :> 0x%04X : %d\n", i, R[i] & MAX_VALUE, R[i]);
    }
    printf("Memory[PC] :> 0x%04X : %d\n", Memory[aux_PC & MAX_VALUE] & MAX_VALUE, Memory[aux_PC & MAX_VALUE]);
    printf("\n\n");
    cout << numInstructions << " instructions executed" << endl
         << endl;
    cout << "LC3 Simulator" << endl;
    cout << "File in simulation => " << fileName << endl
         << endl
         << endl;
    if (PC != orig) //Solo pregunta si desea agregar otro archivo cuando ya haya empezado la simulación
    {
        if (isloadOtherFiles())
            loadFiles();
    }
    if (PC != HALTING_END || (PC == HALTING_END && PC == orig))
        set_stepByStep(); //Pregunta por el tipo de ejecución
}

void controlUnit::set_stepByStep()
{
    char aux = 0; //Variable auxiliar para captura la respuesta a la siguiente pregunta
    string executionType = stepByStep ? "disable" : "enable";
    cin.clear();
    cout << endl
         << "Do you want " << executionType << " step to step execution? (y/N)" << endl;
    cout << ":> ";
    aux = cin.get();
    if (PC != orig) //Soluciona error de lectura de dato, cin esta capturandi un valor basura
        aux = cin.get();
    if (aux == 'y' || aux == 'Y')
    {
        stepByStep = !stepByStep;
        cout << endl
             << "Step-by-step execution " << executionType << "d" << endl;
    }
    cin.clear();
    cin.ignore(10000, '\n');
    systemPause();
    systemClear();
    if (stepByStep)
    {
        if (executionType != (stepByStep ? "disable" : "enable"))
            cout << "Current status of registers" << endl
                 << endl;
    }
}

void controlUnit::isBreakpoint(string nameRegister, short int currentValue, short int breakpointRegister)
{
    if (currentValue == breakpointRegister)
    {
        systemPause();
        systemClear();
        if (nameRegister == "CC")
        {
            printf("Breakpoint found :> %s = %c \n", nameRegister.c_str(), currentValue);
        }
        else
        {
            printf("Breakpoint found :> %s = 0x%04X : %d \n", nameRegister.c_str(), currentValue & MAX_VALUE, currentValue);
        }
        if (PC != orig)
            showRegisters(true);
    }
}

void controlUnit::simulate()
{
    while (PC != HALTING_END)
    {
        if (MAR == OS_KBSR) //Ingresa caracter
        {
            if (kbhit2()) //Si detecta que ingreso un caracter
            {
                pos = OS_KBDR & MAX_VALUE;
                Memory[pos] = getchp(); //Almacena el caracter sin hacer echo
                pos = OS_KBSR & MAX_VALUE;
                Memory[pos] = -32768; //Le lleva al bit[15] del DSR un '1'
            }
            else
            {
                numInstructions++; //Sino ingresa un caracter sigue contacto el numero de instrucciones
            }
            fflush(stdin);
        }
        if (MAR == OS_DSR) //Activa display
        {
            pos = OS_DSR & MAX_VALUE;
            Memory[pos] = -32768; //Le lleva al bit[15] del DSR un '1'
        }
        if (MAR == OS_KBDR)
        {
            pos = OS_KBSR & MAX_VALUE;
            Memory[pos] = 0; //Le lleva al bit[15] del DSR un '0'
        }
        if (MAR == OS_DDR) //Imprime el caracter almacenado en DSR
        {
            pos = OS_DDR & MAX_VALUE;
            printf("%c", Memory[pos]);
            pos = OS_DSR & MAX_VALUE;
            Memory[pos] = 0; //Le lleva al bit[15] del DSR un '0'
        }
        numInstructions++;
        FETCH();
        if (stepByStep) //Mientras el teclado y el display no esten habilitados, si la ejecución paso a paso está activada no pausara el programa, solo en el caso de un breakpoint
            showRegisters(false);
    }
    showRegisters(false);
}

void controlUnit::FETCH()
{
    MAR = PC;
    if (PC == orig) //Cuando inicia la simulación obtine primeros los operandos antes de decodificar, luego se sigue el ciclo normal, primero decodificando y luego obteniendo los operandos
    {
        FETCH_OPERANDS();
    }
    else
    {
        MDR = Memory[PC] & MAX_VALUE;
        IR = MDR;
    }
    instruction.opcode = (MDR >> 12) & 15;
    DECODE();
    if (breakpoints.get_IR() != NULL)
        isBreakpoint("IR", IR, *breakpoints.get_IR());
    //Segun el ciclo de instrucciones aqui iría EVALUATE_ADDRESS(), pero no es necesario porque se hace inderectamente en EVALUATE_ADDRESS() al obtener los offset y obtiene la localidad de memoria que se necesita para procesar la instrución dentro de DECODE() que asu vez está dentro de FECTCH() la anula de alguna forma, evitando hacer esa fase
    MDR = Memory[PC];
    IR = MDR;
    if (instruction.opcode != JSR_JSRR && instruction.opcode != JMP && instruction.opcode != TRAP)
        PC++; //Aumenta el PC la instrucción actual no implica cambiar el valor del PC
    if (breakpoints.get_PC() != NULL)
        isBreakpoint("PC", PC, *breakpoints.get_PC());
}

void controlUnit::DECODE()
{
    FETCH_OPERANDS(); //Optine los operandos de la instrucción
    //NOTA: La fase EXECUTE() y STORE_RESULT(), estan indirectamente implementadas en esta fase, al hacer las operaciones correspondiente a cada instrucción
    IR_BR_JMP_JSR = MDR; //Setea el IR para cuando hay un salto
    if (stepByStep)
        printf("\n%d.----------------------------------------\n    MDR => 0x%04X : ", numInstructions, MDR & MAX_VALUE);
    switch (instruction.opcode) //Evalua que instrucción se va a hacer
    {
    case BR:
        if (PC == orig)
        {
            instruction.BEN = true; //Lo inicializa en true, por si la primera instrucción es un BR
        }
        else
        {
            instruction.BEN = (instruction.N & (CC == 'N')) + (instruction.P & (CC == 'P')) + (instruction.Z & (CC == 'Z')); //BEN<–IR[11] & N + IR[10] & Z + IR[9] & P
        }
        instruction.PCoffset9 = (instruction.PCoffset9 > 255 ? to_a2_complement(instruction.PCoffset9) : instruction.PCoffset9);
        if (stepByStep)
        {
            cout << "BR  ";
            if (instruction.N)
                cout << "N";
            if (instruction.Z)
                cout << "Z";
            if (instruction.P)
                cout << "P";
            printf(", #%d    (%s)\n", instruction.PCoffset9, instruction.BEN ? "true" : "false");
        }
        if (instruction.BEN)
        {
            PC += instruction.PCoffset9;
            if (breakpoints.get_PC() != NULL)
                isBreakpoint("PC", PC, *breakpoints.get_PC());
        }
        break;
    case ADD:
        if (stepByStep)
            printf("ADD R%d, R%d, #%d\n", instruction.DR, instruction.SR1, instruction.imm5 ? instruction.imm5 : instruction.SR2);
        R[instruction.DR] = R[instruction.SR1] + (instruction.is_imm5 ? instruction.imm5 : R[instruction.SR2]); //Si hay imediado a DR le lleva SR1 +imm5, sino le lleva SR1 +SR2
        if (breakpoints.get_R(instruction.DR) != NULL)
            isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], *breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        break;
    case LD:
        instruction.PCoffset9 = (instruction.PCoffset9 > 255 ? to_a2_complement(instruction.PCoffset9) : instruction.PCoffset9) + 1;
        MAR = PC + instruction.PCoffset9;
        MDR = Memory[MAR & MAX_VALUE];
        R[instruction.DR] = MDR;
        if (stepByStep)
        {
            printf("LD  R%d, #%d\n", instruction.DR, instruction.PCoffset9);
            printf("    MAR => 0x%04X : %d\n", MAR & MAX_VALUE, MAR);
            printf("    MDR2 => 0x%04X : %d\n", MDR & MAX_VALUE, MDR);
        }
        if (breakpoints.get_R(instruction.DR) != NULL)
            isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], *breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        break;
    case ST:
        instruction.PCoffset9 = (instruction.PCoffset9 > 255 ? to_a2_complement(instruction.PCoffset9) : instruction.PCoffset9) + 1;
        MAR = PC + instruction.PCoffset9;
        MDR = R[instruction.SR];
        Memory[MAR & MAX_VALUE] = MDR;
        if (stepByStep)
        {
            printf("ST  R%d, #%d\n", instruction.SR, instruction.PCoffset9);
            printf("    MAR => 0x%04X : %d\n", MAR & MAX_VALUE, MAR);
            printf("    MDR2 => 0x%04X : %d\n", MDR & MAX_VALUE, MDR);
            printf("    Storage Memory[0x%04X] => 0x%04X : %d\n", MAR & MAX_VALUE, MDR & MAX_VALUE, MDR);
        }
        break;
    case JSR_JSRR:
        R[7] = PC + 1;          //R7 <- PC
        if (instruction.is_JSR) //JSR
        {
            instruction.PCoffset11 = (instruction.PCoffset11 > 1023 ? to_a2_complement(instruction.PCoffset11) : instruction.PCoffset11) + 1;
            PC = PC + instruction.PCoffset11;
            if (stepByStep)
                printf("JSR #%d\n", instruction.PCoffset11 - 1);
        }
        else //JSRR
        {
            PC = R[instruction.BaseR];
            if (stepByStep)
                printf("JSRR R%d\n", instruction.BaseR);
        }
        if (breakpoints.get_R(7) != NULL)
            isBreakpoint("R7", R[7], *breakpoints.get_R(7));
        if (breakpoints.get_PC() != NULL)
            isBreakpoint("PC", PC, *breakpoints.get_PC());
        break;
    case AND:
        if (stepByStep)
            printf("AND R%d, R%d, #%d\n", instruction.DR, instruction.SR1, instruction.imm5 ? instruction.imm5 : instruction.SR2);
        R[instruction.DR] = R[instruction.SR1] & (instruction.is_imm5 ? instruction.imm5 : R[instruction.SR2]); //Si hay imediado a DR le lleva SR1 and imm5, sino le lleva SR1 and SR2
        if (breakpoints.get_R(instruction.DR) != NULL)
            isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], *breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        break;
    case LDR:
        instruction.offset6 = (instruction.offset6 > 31 ? to_a2_complement(instruction.offset6) : instruction.offset6); //Comprobar si se suma uno en todo
        MAR = R[instruction.BaseR] + instruction.offset6;
        MDR = Memory[(MAR & MAX_VALUE)];
        R[instruction.DR] = MDR;
        if (stepByStep)
        {
            printf("LDR R%d, R%d, #%d\n", instruction.DR, instruction.BaseR, instruction.offset6);
            printf("    MAR => 0x%04X : %d\n", MAR & MAX_VALUE, MAR);
            printf("    MDR2 => 0x%04X : %d\n", MDR & MAX_VALUE, MDR);
        }
        if (breakpoints.get_R(instruction.DR) != NULL)
            isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], *breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        break;
    case STR:
        instruction.offset6 = (instruction.offset6 > 31 ? to_a2_complement(instruction.offset6) : instruction.offset6);
        MAR = R[instruction.BaseR] + instruction.offset6;
        MDR = R[instruction.SR];
        Memory[MAR & MAX_VALUE] = MDR;
        if (stepByStep)
        {
            printf("STR R%d, R%d, #%d\n", instruction.SR, instruction.BaseR, instruction.offset6);
            printf("    MAR => 0x%04X : %d\n", MAR & MAX_VALUE, MAR);
            printf("    MDR2 => 0x%04X : %d\n", MDR & MAX_VALUE, MDR);
            printf("    Storage Memory[0x%04X] => 0x%04X : %d\n", MAR & MAX_VALUE, MDR & MAX_VALUE, MDR);
        }
        break;
    case RTI:
        if (stepByStep)
            cout << "RTI" << endl;
        break;
    case NOT:
        if (stepByStep)
            printf("NOT R%d, R%d\n", instruction.DR, instruction.SR_NOT);
        R[instruction.DR] = ~(R[instruction.SR_NOT]);
        if (breakpoints.get_R(instruction.DR) != NULL)
            isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], *breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        break;
    case LDI:
        instruction.PCoffset9 = (instruction.PCoffset9 > 255 ? to_a2_complement(instruction.PCoffset9) : instruction.PCoffset9) + 1; //TODO: Tengo la duda si se aumenta en 1 o no
        MAR = PC + instruction.PCoffset9;
        MDR = Memory[MAR & MAX_VALUE];
        if (stepByStep)
        {
            printf("LDI R%d, #%d\n", instruction.DR, instruction.PCoffset9);
            printf("    MAR => 0x%04X : %d\n", MAR & MAX_VALUE, MAR);
            printf("    MDR2 => 0x%04X : %d\n", MDR & MAX_VALUE, MDR);
        }
        MAR = MDR;
        MDR = Memory[MAR & MAX_VALUE];
        if (stepByStep)
        {
            printf("    MDR3 => 0x%04X : %d\n", MDR & MAX_VALUE, MDR);
        }
        R[instruction.DR] = MDR;
        if (breakpoints.get_R(instruction.DR) != NULL)
            isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], *breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        break;
    case STI:
        instruction.PCoffset9 = (instruction.PCoffset9 > 255 ? to_a2_complement(instruction.PCoffset9) : instruction.PCoffset9) + 1;
        MAR = PC + instruction.PCoffset9;
        MDR = Memory[MAR & MAX_VALUE];
        if (stepByStep)
        {
            printf("STI R%d, #%d\n", instruction.SR, instruction.PCoffset9 - 1);
            printf("    MAR1 => 0x%04X : %d\n", MAR & MAX_VALUE, MAR);
            printf("    MDR2 => 0x%04X : %d\n", MDR & MAX_VALUE, MDR);
        }
        MAR = MDR;
        MDR = R[instruction.SR];
        Memory[MAR & MAX_VALUE] = MDR;
        if (stepByStep)
        {
            printf("    MAR2 => 0x%04X : %d\n", MAR & MAX_VALUE, MAR);
            printf("    MDR3 => 0x%04X : %d\n", MDR & MAX_VALUE, MDR);
            printf("    Storage Memory[0x%04X] => 0x%04X : %d\n", MAR & MAX_VALUE, MDR & MAX_VALUE, MDR);
        }
        break;
    case JMP:
        if (stepByStep)
            printf("JMP R%d\n", instruction.BaseR);
        PC = R[instruction.BaseR];
        if (breakpoints.get_PC() != NULL)
            isBreakpoint("PC", PC, *breakpoints.get_PC());
        break;
    case RESERVED:
        if (stepByStep)
            cout << "RESERVED" << endl;
        break;
    case LEA:
        instruction.PCoffset9 = (instruction.PCoffset9 > 255 ? to_a2_complement(instruction.PCoffset9) : instruction.PCoffset9) + 1;
        if (stepByStep)
            printf("LEA R%d, #%d\n", instruction.DR, instruction.PCoffset9 + 1);
        R[instruction.DR] = PC + instruction.PCoffset9;
        if (breakpoints.get_R(instruction.DR) != NULL)
            isBreakpoint("R" + to_string(instruction.DR), R[instruction.DR], *breakpoints.get_R(instruction.DR));
        setCC(R[instruction.DR]);
        break;
    case TRAP:
        instruction.trapvect8 = (instruction.trapvect8 > 127 ? to_a2_complement(instruction.trapvect8) : instruction.trapvect8);
        MAR = instruction.trapvect8;
        MDR = Memory[MAR & MAX_VALUE];
        R[7] = PC + 1;
        PC = MDR;
        if (stepByStep)
        {
            printf("TRAP    0x%04X\n", instruction.trapvect8);
            printf("    MAR => 0x%04X : %d\n", MAR & MAX_VALUE, MAR);
            printf("    MDR2 => 0x%04X : %d\n", MDR & MAX_VALUE, MDR);
        }
        if (breakpoints.get_PC() != NULL)
            isBreakpoint("PC", PC, *breakpoints.get_PC());
        break;
    default:
        if (stepByStep)
            cout << "Instruction error\n"
                 << endl;
        break;
    }
}

void controlUnit::FETCH_OPERANDS()
{
    //Se organiza segun el orden de aparición  de la tabla que hay en el libro "Introduction to computing systems" de la casa editorial Mc Graw Hill, pag 119
    instruction.DR = (MDR & 3584) >> 9;            //Hace una AND entre MDR y 0000 1110 0000 0000  y lo desplaza 9 posiciones hacia la derecha para obtener DR
    instruction.SR1 = (MDR & 448) >> 6;            //Hace una AND entre MDR y 0000 0001 1100 0000 y lo desplaza 6 posiciones hacia la derecha para obtener SR1
    instruction.is_imm5 = (MDR & 32) >> 5;         //Hace una AND entre MDR y 0000 0000 0010 0000 y lo desplaza 5 posiciones hacia la derecha para obtener la bandera para saber si hay imm5 (inmediato)
    instruction.SR2 = MDR & 7;                     //Hace una AND entre MDR y 0000 0000 0000 0111, para obtener a SR2
    instruction.imm5 = to_a2_complement(MDR & 31); //Hace una AND entre MDR y 0000 0000 0001 1111, para obtener a imm5
    instruction.N = (MDR >> 11) & 1;               //Hace una AND entre MDR y 0000 1000 0000 0000 y lo desplaza 11 posiciones hacia la derecha para obtener N
    instruction.Z = (MDR >> 10) & 1;               //Hace una AND entre MDR y 0000 0100 0000 0000 y lo desplaza 10 posiciones hacia la derecha para obtener N
    instruction.P = (MDR >> 9) & 1;                //Hace una AND entre MDR y 0000 0010 0000 0000 y lo desplaza 11 posiciones hacia la derecha para obtener N
    instruction.PCoffset9 = MDR & 511;             //Hace una AND entre MDR y 0000 0001 1111 1111, para obtener PCoffset9
    instruction.BaseR = (MDR & 448) >> 6;          //Hace una AND entre MDR y 0000 0001 1100 0000 y lo desplaza 6 posiciones hacia la derecha para obtener BaseR
    instruction.is_JSR = (MDR & 2048) >> 11;       //Hace una AND entre MDR y 0000 1000 0000 0000 y lo desplaza 11 posiciones hacia la derecha para saber si la instrucción es JSR
    instruction.is_JSRR = (MDR & 2048) >> 11;      //Hace una AND entre MDR y 0000 1000 0000 0000 y lo desplaza 11 posiciones hacia la derecha para saber si la instrucción es JSRR
    instruction.PCoffset11 = MDR & 2047;           //Hace una AND entre MDR y 0000 0111 1111 1111, para obtener PCoffset11
    instruction.offset6 = MDR & 63;                //Hace una AND entre MDR y 0000 0000 0011 1111, para obtener offset6
    instruction.SR_NOT = (MDR & 448) >> 6;         //Hace una AND entre MDR y 0000 0001 1100 0000 y lo desplaza 6 posiciones hacia la derecha para obtener SR_NOT
    instruction.SR = (MDR & 3584) >> 9;            //Hace una AND entre MDR y 0000 1110 0000 0000  y lo desplaza 9 posiciones hacia la derecha para obtener SR
    instruction.trapvect8 = MDR & 255;             //Hace una AND entre MDR y 0000 0000 1111 1111, para obtener trapvec8
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
    if (breakpoints.get_CC() != NULL)
        isBreakpoint("CC", CC, *breakpoints.get_CC());
}

void controlUnit::loadFiles()
{
    file fileToSimulate; //Crea un objeto de tipo file, para carga el archivo a simular en la memoria
    isMemoryReset();     //Pregunta sin desea resetea la memoria
    systemPause();
    systemClear();
    if (fileToSimulate.selectPathType()) //Proceso de carga del archivo
    {
        fileToSimulate.loadToMemory(Memory); //Carga el archivo en la memoria
        if (isloadOtherFiles())              //Pregunta si desea cargar más archivo
        {
            loadFiles(); //Carga el otro archivo
        }
        else
        {
            isBreakpointsReset(); //Pregunta si desea resetear los breakpoints
            systemPause();
            systemClear();
            this->orig = fileToSimulate.get_orig();         //Carga el origen del nuevo archivo
            this->fileName = fileToSimulate.get_fileName(); //Carga el nombre del nuevo archivo
            initialize();                                   //Incializa el PC y el MDR
            simulate();                                     //Inicia simulación
            fileToSimulate.~file();                         //Elimina el objeto porque ya no se utilizará, ya fue cargado en memoria
        }
    }
}

short int controlUnit::to_a2_complement(short int num)
{
    if (num > 15 && num < 32) //imm5
    {
        num = (num & 15) - 16;
    }
    else if (num > 31 && num < 64) //offset6
    {
        num = (num & 31) - 32;
    }
    else if (num > 127 && num < 255) //trapvect8
    {
        num = (num & 127) - 128;
    }
    else if (num > 255 && num < 511) //PCoffset9
    {
        num = (num & 255) - 256;
    }
    else if (num > 1023 && num < 2048) //PCoffset11
    {
        num = (num & 1023) - 1024;
    }
    return num;
}

void controlUnit::isMemoryReset()
{
    systemClear();
    char reset;
    cin.ignore(); //Limpia Buffer
    cout << "Do you want memory reset? (y/N)" << endl;
    cout << ":> ";
    reset = cin.get(); //TODO: Despues de
    cin.clear();
    cin.ignore(10000, '\n');
    if (reset == 'y' || reset == 'Y')
    {
        short int Memory_aux[65536] = {0};
        resetRegisters();
        file LC3_OS("LC3_OS/lc3os.obj"); //Lee el sistema operativo de la LC3
        LC3_OS.loadToMemory(Memory_aux); //Carga a la memoria del LC3 su sistema operativo
        //LC3_OS.~file();                  //Destruye el objeto, porque ya se cargo en memoria el systema operativo y no se utilizará más
        for (int i = 0; i < MAX_VALUE; i++)
        {
            Memory[i] = Memory_aux[i];
        }
        cout << "Memory reseted and LC3_OS loaded successfully" << endl;
    }
}

void controlUnit::isBreakpointsReset()
{
    char reset;
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "Do you want breakpoints reset? (y/N)" << endl;
    cout << ":> ";
    reset = cin.get();
    if (reset == 'y' || reset == 'Y')
    {
        cin.clear();
        cin.ignore(10000, '\n');
        breakpoints.resetBreakpoints(); //Elimina los anteriores breakpoints
        breakpoints.setBreakpoints();   //Crea de nuevo un objeto breakpoint
    }
    else
    {
        breakpoints.show_all();
    }
}

void controlUnit::resetRegisters()
{
    instruction = {0};
    for (int i = 0; i < 8; i++)
    {
        R[i] = 0;
    }
    PC = 0;
    IR = 0;
    PSR = 0;
    CC = 0;
    stepByStep = false;
    numInstructions = 0;
    IR_BR_JMP_JSR = 0;
}