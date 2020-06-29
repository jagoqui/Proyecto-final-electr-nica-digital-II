#include "../include/globals.h"
#include <iostream> //Libreria para el manejo de perifericos
#include <string>   //Libreia para el manejo de string´s
#include <fstream>  //Biblioteca para manejar archivos
#include <stdlib.h> //Para manejar new y delete
#include <stdio.h>  //Bibliteca para manejo de streams

using namespace std;

void globals::systemClear()
{
    string OperatingSystem = SO;
    if (OperatingSystem == "Windows")
    {
        system("cls");
    }
    else
    {
        system("clear");
    }
}
void globals::systemPause()
{
    printf("\nPress 'Enter' to continue: ... ");
    fflush( stdout ); //Libera buffer
    while (!kbhit()); //Se queda a la espera de que ingrese un caracter y envie enter
    getchp();
    fflush( stdout ); //Libera buffer
}
bool globals::isloadOtherFiles()
{
    char loadOtherFile;
    cout << "You want to add another file? (y/N)" << endl
         << ":> ";
    fflush( stdout ); //Libera buffer
    while (!kbhit()); //Se queda a la espera de que ingrese un caracter y envie enter
    loadOtherFile = getchp();
    if (loadOtherFile == 'Y' || loadOtherFile == 'y')
        return true;
    return false;
}
bool globals::openWEB_API()
{
    string OperatingSystem = SO;
    if (OperatingSystem == "Windows")
        return system("start ../WEBAPP/HTML/index.html");
    return system("xdg-open ../WEBAPP/HTML/index.html");
    // sudo apt install xdg-utils
}
