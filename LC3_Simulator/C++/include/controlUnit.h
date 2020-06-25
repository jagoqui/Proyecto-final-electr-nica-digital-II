#ifndef CONTROLUNIT_H
#define CONTROLUNIT_H

#include "breakpoint.h"
#include "globals.h"
#include "file.h"
#include "conio.h"

//Constantes para manejar las etiquetas de las instrucciones
const short int BR = 0;
const short int ADD = 1;
const short int LD = 2;
const short int ST = 3;
const short int JSR_JSRR = 4;
const short int AND = 5;
const short int LDR = 6;
const short int STR = 7;
const short int RTI = 8;
const short int NOT = 9;
const short int LDI = 10;
const short int STI = 11;
const short int JMP = 12;
const short int RESERVED = 13;
const short int LEA = 14;
const short int TRAP = 15;

//Estructuara para el manejo del código ISA de cada instrucción
struct INSTRUCTION
{
    bool BEN;            //Bandera para saber si cumple el BR
    unsigned int opcode; //Código de operación
    //Acceso a los registros, R0, R2, R3, R4, R5, R6 y R7
    short int BaseR;     //Se utiliza en JMP, JSRR, LDR y STR
    unsigned int DR;     //Se Utiliza en ADD, AND, LD, LDR, LEA y NOT
    unsigned int SR;     //Se utiliza en ST, STI y STR
    unsigned int SR_NOT; //Se utiliza en NOT
    unsigned int SR1;    //Se utilza en AND y ADD
    unsigned int SR2;    //Se utilza en AND y ADD
    //-------------------------------------------------
    bool is_JSR;          //Bandera para saber si la isntrucción es JSR
    bool is_JSRR;         //Bandera para saber si la isntrucción es JSRR
    bool is_imm5;         //Bandera para saber si la isntrucción tiene valor inmediato
    short int imm5;       //Valor imediato, se utiliza en AND Y ADD
    unsigned int N, Z, P; //Estado de los Flip-Flops activados, se utiliza en BR
    //PCoffset =PC +offset
    short int PCoffset11; //PCoffset de 11 bits, se utiliza en JSR
    short int PCoffset9;  //PCoffset de 9 bits, se utiliza en BR, LD, LEA, ST y STI
    short int offset6;    //Toma el valor de un registro le suma el offset y se suma al PC, se utiliza en LDR y STR
    unsigned trapvect8;   //Vector trap generado, se utiliza en los traps
    short int result;     //Resultado de la instrución
};

class controlUnit : public globals //Clase para el control de las instrucciones
{
    //Atributos
private:
    conio console;                 //Crea el objeto para manejar el IO de la consola
    string fileName;               //Nombre del archivo cargado
    breakpoint breakpoints;        //Crea un objeto tipo break point para setear los breakpoint
    INSTRUCTION instruction = {0}; //Crea una instacia de la estructura INSTRUCTION
    short int *Memory;             //Memoria del LC3
    short int orig;                //Posición de la memoria en donde inicia el programa
    short int R[8] = {0};          //Registros => R0, R2, R3, R4, R5, R6 y R7
    short int PC = 0;              //Program couter
    short int IR = 0;              //Instruction register
    short int PSR = 0;
    char CC = 0;                      //Flip-Flops que estan activado para controlar los branch´s
    short int MAR;                    //Memory address register
    short int MDR;                    //Memory data register
    bool stepByStep = false;          //Bandera para activar la ejecución paso a paso
    unsigned int numInstructions = 0; //Número de instrucciones ejecutadas
    //Metodos
public:
    controlUnit(short int *memory, unsigned int orig, string fileName); //Constructor
    ~controlUnit(){};                                                   //Destructor
private:
    //Metodos para el ciclo de instrucciones
    void initialize(); //Inicializa los valores para el ciclo de intruciones
    void simulate();   //Hace el ciclo de instrucciones del LC3
    void FETCH();      //Buscar y traer, obtiene la instruccion siguiente de la memoria y la carga en el regitro de instrucción (IR)
    void DECODE();     //Examina la instrucción para averiguar que se está pidiendo a ahcer a la microarquitectura. Se decofica el opcode para saber a que isntrucción corresponde
    //Segun el ciclo de instrucciones aqui iría EVALUATE_ADDRESS(), pero no es necesario porque se hace inderectamente en FETCH_OPERANDS() al obtener los offset y obtiene la localidad de memoria que se necesita para procesar la instrución dentro de DECODE() que asu vez está dentro de FETCH() la anula de alguna forma, evitando hacer esa fase
    void FETCH_OPERANDS(); //Buscar y traer los operandos, obtiene los operandos de origen necesarios para procesar la información
    //A igual que con la fase EVALUATE_ADDRESS(), la fase siguiente, EXECUTE(), de nuevo por la forma en que se programó el ciclo de instrucciones, en este caso de nuevo aniñado dentro de otra fase, hace que se ejecute las operaciones correspondientes a esa fase de forma indirecta y que no sea precendible el uso de ésta.
    //De nuevo la siguiente fase, STORE_RESULT(), se hace inderectamente en la fase DECODE()
    //METODOS SECUNDARIOS para hacer calculos y obtener valores de registro
    void setCC(short int result);                                                                 //Calcula si la el resultado de instrucción fue negativo, positivo y cero
    void isBreakpoint(string nameRegister, short int currentValue, short int breakpointRegister); //Buscar si el valor actual del registro es un breakpoint
    void showRegisters(bool isBreakpoint);                                                        //Muestra el estado actual de los regritros
    void loadFiles();                                                                             //Carga otros archivos
    void set_stepByStep();                                                                        //Setea la bandera de activación de ejecución paso a paso
    short int to_a2_complement(short int num);                                                    //Convierte un número a complemento a2
    void isMemoryReset();                                                                         //Pregunta si desea resetear la memoria
    void isBreakpointsReset();                                                                    //Pregunta si desea reseterar los breakpoints
    void resetRegisters();                                                                        //Resetea todos lo registros
    void IN();                                                                                    //Detecta si se ingreso un caracter y lo almacena en KBDR
    void OUT();                                                                                   //Detecta si el display esta listo para mostrar datos
};
#endif