
#ifndef __PCFACTORY_H__
#define __PCFACTORY_H__

#include <iostream>
#include "Mouse.h"
#include "KeyBoard.h"

class PcFactory
{
public:
    virtual Mouse* CreateMouse(void) = 0;
    virtual KeyBoard* CreateKeyBoard(void) = 0;

    virtual void RecycleMouse(Mouse* mouse) = 0;
    virtual void RecycleKeyBoard(KeyBoard* keyBoard) = 0;
};



#endif
