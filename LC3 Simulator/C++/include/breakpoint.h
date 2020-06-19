#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "globals.h"

using namespace std;

//Clase para setear breakpoints
class breakpoint : public globals
{
    //Atributos
private:
    short int PC = 0; //Progrma counter
    short int x = 0; //Origin
    short int IR = 0; //Instructions
    short int PSR = 0;
    char CC = 0; //NZP flip-flops
    short int R[8]= {0}; //Registers
    //Metodos
public:
    breakpoint();
    ~breakpoint(){};
    void setBreakpoints();                         //Setear breakpoints
    short int getString_to_hex(); //Lee el valor de los breakpoits y lo pasa a numeros
    bool checkRange(int num);
    short  int get_PC() { return PC; };
    short  int get_x() { return x; };
    short  int get_IR() { return IR; };
    short  int get_PSR() { return PSR; };
    char get_CC() { return CC; };
    short int get_R(int i) { return R[i]; };
};

#endif