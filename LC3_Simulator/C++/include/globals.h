#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>   //Libreria para el manejo de string´s
#include <fstream>  //Biblioteca para manejar archivos
#include <stdlib.h> //Para manejar new y delete
#include <stdio.h>  //Bibliteca para manejo de streams

//Detecta el sistema operativo
#ifdef __linux__
#define SO "Linux"
#elif _WIN32
#define SO "Windows"
#endif

using namespace std;
//Clase para el manejo de las variables y funciones globales
class globals
{
private:
    //Metodos
public:
    globals(){};
    ~globals(){};
    void systemClear(); //Limpia pantalla
    void systemPause(); //Pausa la ejecucíon
    bool isloadOtherFiles(); //Pregunta si desea cargar otro archivo
    bool openWEB_API(); //Abre la aplicación WEB
};

#endif