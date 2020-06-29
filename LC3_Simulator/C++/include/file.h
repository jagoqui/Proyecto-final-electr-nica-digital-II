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
    string fileName = "";         //Nombre del archivo cargado
    string url_path;         //Dirección del archivo
    ifstream file_obj;       //Archivo
    short int *instructions; //instrucciones, sera un arreglo dinámico
    int numInstructions;     //Número de instrucciones
    unsigned int orig;       //Posición en donde empezará el programa
    //Metodos
public:
    file(){};                                  //Constructor sin parametros
    file(string _url_path);                    //Constructor con parametros
    ~file(){};                                 //Destructor
    void loadToMemory(short int *memory);      //Carga las instrucciones a la memoria
    short int get_orig() { return orig; };     //Devuele el .orig del programa
    bool selectPathType();                     //Selecciona el tipo de ruta del archivo
    string get_fileName() { return fileName; } //Retorna el nombre del archivo
private:
    bool read();          //Lee el archivo
    bool checkFileType(); //Verifica que el archivo que cargó tenga extención .obj
    void set_fileName();  //Setea el nombre del archivo
};

#endif
