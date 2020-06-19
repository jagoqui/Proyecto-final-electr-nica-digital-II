#ifndef CONTROLUNIT_H
#define CONTROLUNIT_H

#include "breakpoint.h"
#include "globals.h"
#include "file.h"

//Estructuara para el manejo del código ISA de cada instrucción
struct INSTRUCTION
{
    bool BEN;
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
    breakpoint breakpoints;        //Crea un objeto tipo break point para setear los breakpoint
    INSTRUCTION instruction = {0}; //Crea una instacia de la estructura INSTRUCTION
    short int *Memory;             //Memoria del LC3
    short int orig;                //Posición de la memoria en donde inicia el programa
    short int R[8] = {0};          //Registros => R0, R2, R3, R4, R5, R6 y R7
    short int PC = 0;              //Program couter
    short int IR = 0;           //Instruction register
    short int PSR = 0;
    char CC = ' ';    //Flip-Flops que esta activado para controlar los branch´s
    short int MAR; //Memory address register
    short int MDR; //Memory data register
    //Metodos
public:
    controlUnit(short int *memory, unsigned int orig); //Constructor
    ~controlUnit(){};                                  //Destructor
private:
    //Metodos para el ciclo de instrucciones
    void simulate(); //Hace el ciclo de instrucciones del LC3
    void FETCH();    //Buscar y traer, obtiene la instruccion siguiente de la memoria y la carga en el regitro de instrucción (IR)
    void DECODE();   //Examina la instrucción para averiguar que se está pidiendo a ahcer a la microarquitectura. Se decofica el opcode para saber a que isntrucción corresponde
    //Segun el ciclo de instrucciones aqui iría EVALUATE_ADDRESS(), pero no es necesario porque se hace inderectamente en FETCH_OPERANDS() al obtener los offset y obtiene la localidad de memoria que se necesita para procesar la instrución dentro de DECODE() que asu vez está dentro de FETCH() la anula de alguna forma, evitando hacer esa fase
    void FETCH_OPERANDS(); //Buscar y traer los operandos, obtiene los operandos de origen necesarios para procesar la información
    //A igual que con la fase EVALUATE_ADDRESS(), la fase siguiente, EXECUTE(), de nuevo por la forma en que se programó el ciclo de instrucciones, en este caso de nuevo aniñado dentro de otra fase, hace que se ejecute las operaciones correspondientes a esa fase de forma indirecta y que no sea precendible el uso de ésta.
    //De nuevo la siguiente fase, STORE_RESULT(), se hace inderectamente en la fase DECODE()
    //METODOS SECUNDARIOS para hacer calculos y obtener valores de registro
    void setCC(short int result);                                                                 //Calcula si la el resultado de instrucción fue negativo, positivo y cero
    void isBreakpoint(string nameRegister, short int currentValue, short int breakpointRegister); //Buscar si el valor actual del registro es un breakpoint
    void showRegisters();                                                                         //Muestra el estado actual de los regritros
    void loadFiles();                                                                             //Carga otros archivos
};
#endif