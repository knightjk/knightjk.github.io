
#include "HpFactory.h"

using namespace std;

Mouse* HpFactory::CreateMouse(void)
{
    return new HpMouse;
}
KeyBoard* HpFactory::CreateKeyBoard(void)
{
    return new HpKeyBoard;
}

void HpFactory::RecycleMouse(Mouse* mouse)
{
    if(mouse != NULL)
        delete mouse;
}

void HpFactory::RecycleKeyBoard(KeyBoard* keyBoard)
{
    if(keyBoard != NULL)
        delete keyBoard;
}

