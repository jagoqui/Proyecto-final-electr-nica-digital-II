#include "../include/breakpoint.h"
#include <iostream>      //Libreria para el manejo de perifericos
#include <string>        //Libreia para el manejo de string´s
#include <stdlib.h>      //Para manejar new y delete
#include <stdio.h>       //Libreria para el manejo de streams
#include <bits/stdc++.h> //Libreria para pasar de string a hex y viceversa

using namespace std;

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
    cin.ignore();
    if (addBreakpoint == 'y' || addBreakpoint == 'Y')
    {
        systemClear();
        cout << "Select register" << endl
             << endl;
        cout << "1. PC" << endl;
        cout << "2. x" << endl;
        cout << "3. IR" << endl;
        cout << "4. CC" << endl;
        cout << "5. R0, R1, R2, R3, R4, R5, R6, R7" << endl;
        cout << endl
             << ":> ";
        selRegister = cin.get();
        systemClear();
        if (selRegister != '5')
            cout << "Enter the breakpoint, in hex the format is '0xValueHex' or in integer, 'ValueInt'" << endl;
        switch (selRegister)
        {
        case '1': //PC
            cout << "PC :> ";
            cin.ignore(); //TODO: Cuidado con ésto
            PC = getString_to_hex();
            break;
        case '2': //x
            cout << "x :> ";
            cin.ignore(); //TODO: Cuidado con ésto
            x = getString_to_hex();
            break;
        case '3': //IR
            cout << "IR :> ";
            cin.ignore(); //TODO: Cuidado con ésto
            IR = getString_to_hex();
            break;
        case '4': //CC
            char aux;
            cin.ignore();
            cout << "Enter N, Z or P" << endl;
            cout << "CC :> ";
            aux = cin.get();
            if (aux == 'N' || aux == 'P' || aux == 'Z' || aux == 'n' || aux == 'p' || aux == 'z')
            {
                if (aux == 'n' || aux == 'p' || aux == 'z')
                {
                    aux = toupper(aux);
                }
                CC = aux;
                cin.ignore();
            }
            else
            {
                systemClear();
                cin.ignore();
                cout << "Invalid value flip-flop" << endl
                     << endl;
                CC = 0;
            }
            break;
        case '5': //R
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
                int i = Rnumber -'0';
                cout << "Enter the breakpoint, in hex the format is '0xValueHex' or in integer, 'ValueInt'" << endl;
                cout << "R" << i << " :> ";
                R[i] = getString_to_hex(); // atoi (pasa de ASCII a int)
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
            break;
        }
        cout << endl
             << "Breakpoints:" << endl
             << endl;
        printf("PC :> 0x%x : %d\n", PC, PC);
        printf("x  :> 0x%x : %d\n", x, x);
        printf("IR :> 0x%x : %d\n", IR, IR);
        printf("CC :> 0x%x : %c\n", CC, CC);
        for (int i = 0; i < 8; i++)
        {
            printf("R%d :> 0x%x : %d\n", i, R[i], R[i]);
        }
        setBreakpoints();
    }
}
short int breakpoint::getString_to_hex()
{
    string hexStr = "";
    string hexIdicator = "0x";
    long int aux;
    getline(cin, hexStr);
    aux = stoi(hexStr);
    if (hexStr.substr(0, 2) == hexIdicator) //Si entra aquí es porque ingresó un hexadecimal
    {
        stringstream ss;
        ss << hex << hexStr; //Pasa el hexadecimal en string a entero
        ss >> aux;
        if(aux>32767) //Detecta si es un número negativo para hacer la conversión a complemento a2
        aux = ((~aux +1) & 65535)*-1;
        if (checkRange(aux))
        {
            if ((ss.str() == "0" && (hexStr == "0x" || hexStr == "0x00" || hexStr == "0x000" || hexStr == "0x0000")) || aux != 0) //Verifica que se ingresó el hexadecimal en formato correcto
            {
                return aux;
            }
            else
            {
                cout << "Invalid input hex" << endl;
                return 0;
            }
        }
        return 0; //Detecta que se sale del rango
    }
    else //Ingresó valor númerico
    {
        if (to_string(aux) != hexStr) //Detecta que ingreso un valor erroreno
        {
            cout << "Invalid input data" << endl;
            return 0;
        }
        if (checkRange(aux))
        {
            return aux;
        }
        return 0; //Detecta que se sale del rango
    }
}
bool breakpoint::checkRange(int num)
{
    if (num >= -32768 && num <= 32767) 
        return true;
    cout << "Invalid input, out range (-32768 to 32767)dec (0x0 to 0xFFFF)" << endl;
    return false;
}