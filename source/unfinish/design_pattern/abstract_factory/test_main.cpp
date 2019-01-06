//g++ HpMouse.cpp DellMouse.cpp HpKeyBoard.cpp AcerKeyBoard.cpp DellFactory.cpp HpFactory.cpp AcerFactory.cpp test_main.cpp -o test_main

#include <iostream>
#include "DellFactory.h"
#include "HpFactory.h"
#include "AcerFactory.h"

using namespace std;

int main(int argc, char* argv[])
{
    DellFactory dellFactory;
    HpFactory hpFactory;
    AcerFactory acerFactory;

    Mouse* dellMouse;
    Mouse* hpMouse;
    KeyBoard* acerKeyBoard;
    KeyBoard* hpKeyBoard;

    dellMouse = dellFactory.CreateMouse();
    
    dellMouse->RollWheel();

    dellFactory.RecycleMouse(dellMouse);

    hpMouse = hpFactory.CreateMouse();
    hpKeyBoard = hpFactory.CreateKeyBoard();
    
    hpMouse->Click();
    hpKeyBoard->Typing("hehesimida");

    hpFactory.RecycleKeyBoard(hpKeyBoard);
    hpFactory.RecycleMouse(hpMouse);

    acerKeyBoard = acerFactory.CreateKeyBoard();
    acerKeyBoard->Typing("hehenimiea");
    acerFactory.RecycleKeyBoard(acerKeyBoard);

    return 1;
}




