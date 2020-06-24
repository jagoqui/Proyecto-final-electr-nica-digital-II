#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "globals.h"

using namespace std;

//Clase para setear breakpoints
class breakpoint : public globals
{
    //Atributos
private:
    short int *PC = NULL; //Progrma counter
    short int *IR = NULL; //Instructions
    short int *PSR = NULL;
    char *CC = NULL;          //NZP flip-flops
    short int *R[8] = {NULL}; //Registers
    //Metodos
public:
    breakpoint();
    ~breakpoint(){};
    void setBreakpoints();                        //Setear breakpoints
    void getString_to_hex(short int &ptr);        //Lee el valor de los breakpoits y lo pasa a numeros
    bool isNULL(string value);                    //Comprueba si el valor ingresado fue NULL
    void searchBreakpointToDelete(short int ptr); //Busca el breakpoint a eliminar
    bool checkRange(int num);                     //Valida los rangos permitidos
    short int *get_PC() { return PC; };           //Retorna el breakpoint para PC
    short int *get_IR() { return IR; };           //Retorna el breakpoint para IR
    short int *get_PSR() { return PSR; };         //Retorna el breakpoint para PSR
    char *get_CC() { return CC; };                //Retorna el breakpoint para CC
    short int *get_R(int i) { return R[i]; };     //Retorna el breakpoint para los registros
    void show_all();                              //Muestra en consola todos los breakpoints
    void resetBreakpoints();                      //LLeva a NULL todos los breakpoints
};

#endif