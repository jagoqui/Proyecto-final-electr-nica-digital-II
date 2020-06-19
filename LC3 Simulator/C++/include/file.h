#ifndef FILE_H
#define FILE_H

#include "globals.h"
#include <string>   //Libreia para el manejo de string´s
#include <fstream>  //Biblioteca para manejar archivos
#include <stdlib.h> //Para manejar new y delete
#include <stdio.h>  //Bibliteca para manejo de streams

using namespace std;
//Clase para el manejo de los archivos
class file : public globals 
{
    //Atributos
private:
    string url_path;            //Dirección del archivo
    ifstream file_obj;          //Archivo
    short int *instructions; //instrucciones, sera un arreglo dinámico
    int numInstructions;        //Número de instrucciones
    unsigned int orig;          //Posición en donde empezará el programa
    //Metodos
public:
    file(); //Constructor sin parametros
    file(string _url_path); //Constructor con parametros
    ~file(){}; //Destructor
    void loadToMemory(short int *memory);
    short int get_orig(); //Devuele el .orig del programa
    void selectPathType(); //Selecciona el tipo de ruta del archivo
private:
    void read(); //Lee el archivo
};

#endif
