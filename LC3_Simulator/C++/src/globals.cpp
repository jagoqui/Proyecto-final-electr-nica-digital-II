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
    if(OperatingSystem == "Windows")
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
    while (getchar() != '\n');
}
bool globals::isloadOtherFiles()
{
    char loadOtherFile;
    cout << "You want to add another file? (y/N)" << endl
         << ":> ";
    loadOtherFile = cin.get();
    if(loadOtherFile == 0)
        cout<<"Error"<<endl;
    if (loadOtherFile == 'Y' || loadOtherFile == 'y')
        return true;
    return false;
}
bool globals::openWEB_API(){
    string OperatingSystem = SO;
    if(OperatingSystem == "Windows")
        return system("start ../WEBAPP/HTML/index.html");
    return system("xdg-open ../WEBAPP/HTML/index.html");
    // sudo apt install xdg-utils
}
