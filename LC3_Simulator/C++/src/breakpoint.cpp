#include "../include/breakpoint.h"
#include <iostream>      //Libreria para el manejo de perifericos
#include <string>        //Libreia para el manejo de string´s
#include <stdlib.h>      //Para manejar new y delete
#include <stdio.h>       //Libreria para el manejo de streams
#include <bits/stdc++.h> //Libreria para pasar de string a hex y viceversa

using namespace std;

const long int maxValue = 32768;

breakpoint::breakpoint()
{
    setBreakpoints();
}

void breakpoint::setBreakpoints()
{
    char addBreakpoint = 0;
    char selRegister = 0;
    cout << endl
         << "Do you add Breakpoints? (y/N)" << endl;
    cout << ":> ";
    addBreakpoint = cin.get();
    cin.clear();
    cin.ignore(10000, '\n');
    if (addBreakpoint == 'y' || addBreakpoint == 'Y')
    {
        systemClear();
        cout << "Select register" << endl
             << endl;
        cout << "1. PC" << endl;
        cout << "2. IR" << endl;
        cout << "3. CC" << endl;
        cout << "4. R0, R1, R2, R3, R4, R5, R6, R7" << endl;
        cout << endl
             << ":> ";
        selRegister = cin.get();
        systemClear();
        if (selRegister != '5')
            cout << "Enter the breakpoint, in hex the format is '0xValueHex' or in integer, 'ValueInt' (type 'NULL' to delete breakpoint last seted)" << endl;
        switch (selRegister)
        {
        case '1': //PC
            cout << "PC :> ";
            cin.ignore(); //TODO: Cuidado con
            PC = new short int;
            getString_to_hex(*PC);
            break;
        case '2': //IR
            cout << "IR :> ";
            cin.ignore(); //TODO: Cuidado con ésto
            IR = new short int;
            getString_to_hex(*IR);
            break;
        case '3': //CC
            char aux;
            cin.ignore();
            cout << "Enter N, Z or P (0 to delete breakpoint last seted)" << endl;
            cout << "CC :> ";
            aux = cin.get();
            if (aux == 'N' || aux == 'P' || aux == 'Z' || aux == 'n' || aux == 'p' || aux == 'z')
            {
                if (aux == 'n' || aux == 'p' || aux == 'z')
                {
                    aux = toupper(aux);
                }
                CC = new char;
                CC = &aux;
                cin.ignore();
            }
            else if (aux == '0')
            {
                CC = NULL;
                cout << endl
                     << "Breakpoint deleted" << endl;
                cin.clear();
                cin.ignore(10000, '\n');
            }
            else
            {
                systemClear();
                cin.ignore();
                cout << "Invalid value flip-flop" << endl
                     << endl;
            }
            break;
        case '4': //R
            char Rnumber;
            systemClear();
            cin.ignore();
            cout << "Enter register number 0-7" << endl;
            cout << ":> ";
            Rnumber = cin.get();
            systemClear();
            cin.ignore();
            if (Rnumber > 47 && Rnumber < 56)
            {

                int i = Rnumber - '0';
                cout << "Enter the breakpoint, in hex the format is '0xValueHex' or in integer, 'ValueInt'" << endl;
                cout << "R" << i << " :> ";
                R[i] = new short int;
                getString_to_hex(*R[i]);
            }
            else
            {
                systemClear();
                cout << "Invalid number of register" << endl;
            }
            break;
        default:
            systemClear();
            cout << "Ivalid input" << endl
                 << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            break;
        }
        show_all(); //Muestra todos los breakpoints
        setBreakpoints();
    }
}

void breakpoint::getString_to_hex(short int &ptr)
{
    string hexStr = "";
    string hexIdicator = "0x";
    long int aux;
    short int auxToshortInt;
    getline(cin, hexStr);
    bool is_NULL = isNULL(hexStr);
    if (hexStr.substr(0, 1) != "x" && !is_NULL)
    {
        aux = stoi(hexStr);
        if (hexStr.substr(0, 2) == hexIdicator) //Si entra aquí es porque ingresó un hexadecimal
        {
            stringstream ss;
            ss << hex << hexStr; //Pasa el hexadecimal en string a entero
            ss >> aux;
            if (aux > maxValue - 1) //Detecta si es un número negativo para hacer la conversión a complemento a2
            {
                aux = ((~aux + 1) & 65535) * -1;
            }
            auxToshortInt = (short int)aux & 65535;
            if (checkRange(aux))
            {
                if ((ss.str() == "0" && (hexStr == "0x" || hexStr == "0x00" || hexStr == "0x000" || hexStr == "0x0000")) || aux != 0) //Verifica que se ingresó el hexadecimal en formato correcto
                {
                    ptr = auxToshortInt;
                }
                else
                {
                    cout << "Invalid input hex" << endl;
                }
            }
        }
        else //Ingresó valor númerico
        {
            if (to_string(aux) != hexStr) //Detecta que ingreso un valor erroreno
            {
                cout << "Invalid input data" << endl;
            }
            if (checkRange(aux))
            {
                auxToshortInt = (short int)aux & 65535;
                ptr = auxToshortInt;
            }
        }
    }
    else if (!is_NULL)
    {
        cout << "Invalid input" << endl
             << endl;
    }
    else
    {
        if (&ptr == PC && PC)
        {
            PC = NULL;
        }
        else if (&ptr == IR)
        {
            IR = NULL;
        }
        else
        {
            for (int i = 0; i < 8; i++)
            {
                if (&ptr == R[i])
                {
                    R[i] = NULL;
                    i = 7;
                }
            }
        }
        cout << endl
             << "Breakpoint deleted" << endl;
    }
}

bool breakpoint::isNULL(string value)
{
    for (int i = 0; i < (int)value.length(); i++)
    {
        value[i] = toupper(value[i]);
    }
    if (value == "NULL")
        return true;
    return false;
}

bool breakpoint::checkRange(int num)
{
    if (num >= -maxValue && num <= maxValue - 1)
        return true;
    cout << "Invalid input, out range (-32768 to 32767)dec (0x0 to 0xFFFF)" << endl;
    return false;
}

void breakpoint::show_all()
{
    if (PC != NULL || IR != NULL || CC != NULL || R[0] != NULL || R[1] != NULL || R[2] != NULL || R[3] != NULL || R[4] != NULL || R[5] != NULL || R[6] != NULL || R[7] != NULL)
    {
        cout << endl
             << "Breakpoints:" << endl
             << endl;
        if (PC != NULL)
        {
            printf("PC :> 0x%04X : %d\n", *PC & 65535, *PC);
        }
        if (IR != NULL)
        {
            printf("IR :> 0x%04X : %d\n", *IR & 65535, *IR);
        }
        if (CC != NULL)
            printf("CC :> 0x%04X : %c\n", *CC & 65535, *CC);
        for (int i = 0; i < 8; i++)
        {
            if (R[i] != NULL)
                printf("R%d :> 0x%04X : %d\n", i, *R[i] & 65535, *R[i]);
        }
    }
    else
    {
        cout << "No set breakpoits." << endl;
    }
}

void breakpoint::resetBreakpoints(){
    PC =NULL;
    IR = NULL;
    CC = NULL; 
    PSR = NULL;
    for (int i = 0; i < 8; i++)
    {
        R[i]= NULL;
    }
}