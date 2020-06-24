#include "../include/file.h"
#include <iostream> //Libreria para el manejo de perifericos
#include <string>   //Libreia para el manejo de string´s
#include <fstream>  //Biblioteca para manejar archivos
#include <stdlib.h> //Para manejar new y delete
#include <stdio.h>  //Bibliteca para manejo de streams

using namespace std;

file::file(string _url_path)
{
    url_path = _url_path;
    read();
}

bool file::read()
{
    file_obj.open(url_path.c_str(), ifstream::in);
    // Si falla en la carga del archivo
    if (file_obj.fail())
    {
        cout << endl
             << "Fail file load" << endl
             << endl;
        return selectPathType();
    }
    else if (!file_obj.fail())
    {
        set_fileName();
        instructions = new short int[1]; //Crea un arreglo de una posición para poder hacerlo dinamico
        int counter = 0;                 //Contador para el números de instrucciones
        bool flagIs2MSBs = true;         //Bandera para saber si está en los 2 bits más significativos, inicialmete empiez true, porque cada línea la lee de izquierda a derecha
        while (!file_obj.eof())          //Mientras no sea el final del archivo
        {
            instructions[counter] = file_obj.get(); //Obtine los dos primeros bit más significativos de cada instrucción y al terminar de leer pasa el puntero del archivo al siguiente byte
            if (!file_obj.eof())                    //Si el siguiente byte tiene el valor -1, entonces indica que el documento se terminó, etonces en éste caso no seguira guardando caracteres
            {
                if (flagIs2MSBs) //Si esta leyendo los 2 primeros bits más significativos
                {
                    instructions[counter] <<= 8; //Desplaza 8 bits hacia la izquierda
                }
                else //Si esta leyendo los dos primeros bit menos signficativos
                {
                    counter--;
                    instructions[counter] += instructions[counter + 1];
                    instructions[counter] = instructions[counter] & 65535; //Lleva a cero los 16 bits más significativos del short int
                }
                flagIs2MSBs = !flagIs2MSBs; //La bandera cambia de estado, porque va pasar de leer los 2 bit mas significativos a los 2 menos significativos, o viceversa
                counter++;
                //Manejo de la memoria dinámica
                unsigned int *auxInstructions = new unsigned int[counter]; //Crea una variable auxiliar para manejar dinamicamente el vectos con la instrucciones
                for (int i = 0; i < counter; i++)                          //Copia la actual información del vector de instrucciones al vector auxiliar
                {
                    auxInstructions[i] = instructions[i];
                }
                delete[] instructions;                     //Libera el espacio anteriomente reservado por el vector de istrucciones
                instructions = new short int[counter + 1]; //Asigna de nuevo espacio en la memoria para el vector de instrucciones, pero esta vez con una posición más para que pueda almacenar en ésta el próximo caracter del archivo
                for (int i = 0; i < counter; i++)          //Recupera la copia antes hecha en el vector auxiliar
                {
                    instructions[i] = auxInstructions[i];
                }
                delete[] auxInstructions; //Libera el espacio en memoria asignado por el vector de instrucciones
            }
        }
        numInstructions = counter - 1; //Le asigna el número de instrucciones del archivo leido
        file_obj.close();              //Cierra el archivo
        cout << "Successful loading" << endl
             << endl;
        return true;
    }
    else
    {
        cout << "Error! not found file, try again or press 'q' to exit program, 'c' to file load cancel" << endl
             << endl;
        return selectPathType();
    }
}

bool file::selectPathType()
{
    string relativePath = "../../../";
    char pathType = 0;
    cout << "Enter path, select path type ('q' : exit program | 'c' : file load cancel)" << endl
         << endl;
    cout << "1.Absolute" << endl;
    cout << "2.Relative (Recommended for file located in the same project directory)" << endl
         << endl
         << ":> ";
    pathType = cin.get();
    systemClear();
    if (pathType == '1')
    {
        cout << "Enter absolute path of file to simulate ";
        cout << ":> ";
        cin.ignore(); //Omite el último salto de linea para que no intervenga con la lectura del dato
        getline(cin, url_path);
        if (checkFileType()) //Verifica la extensión del archivo
        {
            read();
        }
        else
        {
            return selectPathType();
        }
    }
    else if (pathType == '2')
    {
        cout << "Enter Relative path of file to simulate ";
        cout << ":> " << relativePath;
        cin.ignore(); //Omite el último salto de linea para que no intervenga con la lectura del dato en la cadena de caracteres, se debe utilizar cada vez que utilice getline()
        getline(cin, url_path);
        url_path = relativePath + url_path;
        if (checkFileType()) //Verifica la extensión del archivo
        {
            read();
            return true;
        }
        else
        {
            return selectPathType();
        }
    }
    else if (pathType != 'q' && pathType != 'Q' && pathType != 'c' && pathType != 'C')
    {
        cout << "Invalid data, try again or press 'q' to exit program, 'c' to load file cancel" << endl
             << endl;
        return selectPathType();
    }
    else if (pathType == 'c' ||  pathType == 'C')
    {
        cout << "Canceled" << endl
             << endl;
        return false;
    }
    else
    {
        cout << "Exit" << endl;
        exit(1);
    }
    return false;
}

void file::loadToMemory(short int *memory)
{
    orig = instructions[0];             //Carga el la posición de la memoria en donde debe empezar el programa
    int i_max = orig + numInstructions; //Limita el valor máximo que tendra el iterador i denttro del siguiente ciclo
    for (int i = orig; i < i_max; i++)  //Lee la primer instrucción que corresponde al .orig, y apartir de allí almacena en la memoria del sistema operativo las instrucciones
    {
        instructions++;            //Se desplaza a la siguiente instrucción, no almacena el valor del .orig
        memory[i] = *instructions; //Carga cada instrucción en la memoria
    }
    // delete[] instructions; //TODO:Porque sale error cuando se libera la memoria en este caso?
}

bool file::checkFileType()
{

    int sizePath = url_path.size() / sizeof(char); //Obtine el numero de caracteres del string
    if (sizePath > 4)
    {
        string ext = url_path.substr(sizePath - 4, sizePath); //Extensión del archivo
        if (ext == ".obj")
            return true;
        cout << "Invalid file, the file must have extension .obj, try again or press 'q' to exit program, 'c' to load file cancel" << endl
             << endl;
        return false;
    }
    cout<<"Invalid file, try again or press 'q' to exit program, 'c' to load file cancel"<<endl<<endl;
    return false;
}

void file::set_fileName(){
    int size = url_path.length();
    int initialPosName;
    for (int i = 0; i < size; i++)
    {
        if(url_path[i] != 47 && url_path[i] != 92 && url_path[i] != '.') //Si url_path[i] es diferente a (/, \, .)
        {
            initialPosName = i;
            i=size;
        }
    }
    fileName = url_path.substr(initialPosName,size);
}



