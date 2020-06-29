#ifndef CONIO_H
#define CONIO_H

//Librerias
// #include "termios2.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>

using namespace std;

class conio
{
private:
    /* data */
public:
    conio(){};
    ~conio(){};
    int getch(void);  //Función que lee un caracter desde el teclado, sin echo
    int getche(void); //Función que lee un caracter desde el teclado, con echo
    int getchp(void); //Función que lee un caracter pendiente desde el teclado, sin echo
    int kbhit(void);  //Función que espera a que se leea un caracter en el teclado
    int kbhit2(void); //Función que espera a que se leea un caracter en el teclado
};

#endif