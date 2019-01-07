//g++ HpMouse.cpp DellMouse.cpp SimpleMouseFactory.cpp test_main.cpp -o test_main

#include <iostream>
#include "SimpleMouseFactory.h"

using namespace std;

int main(int argc, char* argv[])
{
    SimpleMouseFactory mouseFactory;
    Mouse* hpMouse;
    Mouse* dellMouse;

    hpMouse = mouseFactory.CreateMouse(HpMouseType);

    hpMouse->Click();

    mouseFactory.DestroyMouse(hpMouse);
    return 1;
}




