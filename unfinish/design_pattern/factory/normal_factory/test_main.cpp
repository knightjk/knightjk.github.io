//g++ HpMouse.cpp DellMouse.cpp DellMouseFactory.cpp HpMouseFactory.cpp test_main.cpp -o test_main

#include <iostream>
#include "DellMouseFactory.h"
#include "HpMouseFactory.h"

using namespace std;

int main(int argc, char* argv[])
{
    DellMouseFactory dellMouseFactory;
    HpMouseFactory hpMouseFactory;
    Mouse* mouse;

    mouse = dellMouseFactory.CreateMouse();
    
    mouse->RollWheel();

    dellMouseFactory.DestroyMouse(mouse);

    return 1;
}




