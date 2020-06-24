#include "../include/file.h"
#include "../include/globals.h"
#include "../include/controlUnit.h"
#include <iostream> //Libreria para el manejo de perifericos
#include <string>   //Libreia para el manejo de string´s
#include <stdlib.h> //Para manejar new y delete

#ifdef __linux__
#define SO "Linux"
#elif _WIN32
#define SO "Windows"
#endif

using namespace std;

void loadFiles();              //Cargar archivos
short int Memory[65536] = {0}; //Son 65536 posiciones de memoria que tiene el LC3 , inicializa todo en cero
globals g;

int main(int argc, char *argV[])
{
    g.systemClear();
    cout << "Running in " << SO << endl
         << endl;
    cout << "LC3 SIMULATOR" << endl
         << endl;
    cout << "Init ... preload LC3_OS" << endl;
    g.systemPause();
    g.systemClear();
    file LC3_OS("LC3_OS/lc3os.obj"); //Lee el sistema operativo de la LC3
    LC3_OS.loadToMemory(Memory);     //Carga a la memoria del LC3 su sistema operativo
    LC3_OS.~file();                  //Destruye el objeto, porque ya se cargo en memoria el systema operativo y no se utilizará más
    cout << "Operating system load successful, press only enter key to ative WEB APP mode, or other key to active terminal mode. (enter to submit)" << endl
         << ":> ";
    if (cin.get() == '\n')
    {
        g.systemClear();
        if (g.openWEB_API() == false)
        {
            cout << "Activated WEB APP mode, this program will continue to run in the background, please do not close this commands windows." << endl;
            g.systemPause();
        }
        else
        {
            cout << "URL no found, desactived WEP APP mode, activated terminal mode" << endl;
            loadFiles(); //Carga multiples archivos
        }
    }
    else //Activa modo terminal
    {
        g.systemClear();
        cout << "ACTIVATE TERMINAL MODE !!!" << endl
             << endl
             << endl;
        loadFiles(); //Carga multiples archivos
    }
    for (int i = 12448; i < 12478; i++)
    {
        printf("Memory[0x%04X] => 0x%04X : %d\n", i, Memory[i] & 65535, Memory[i]);
    }
    cout << "Finished simulation" << endl;
    return 0;
}
void loadFiles()
{
    file fileToSimulate;                 //Crea un objeto de tipo file, para carga el archivo a simular en la memoria
    cin.ignore();                        //Como en la siguiente función va a ver lectura de datos, entonces evito posibles problemas, ignorando el enter del último cin.get()
    if (fileToSimulate.selectPathType()) //Proceso de carga del archivo
    {
        fileToSimulate.loadToMemory(Memory); //Carga el archivo en la memoria
        if (g.isloadOtherFiles())            //Pregunta si desea cargar más archivos
            loadFiles();
        cin.ignore();
        controlUnit control(Memory, fileToSimulate.get_orig(), fileToSimulate.get_fileName()); //Inicia etapa de control para hacer el ciclo de instrucciones, iniciando en la direccion de memoria (.orig) en donde inicia el programa
        fileToSimulate.~file();                                                                //Elimina el objeto porque ya no se utilizará, ya fue cargado en memoria
        control.~controlUnit();                                                                //Destruye el objeto
    }
}
