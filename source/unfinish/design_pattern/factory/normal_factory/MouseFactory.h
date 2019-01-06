
#ifndef __MOUSEFACTORY_H__
#define __MOUSEFACTORY_H__

#include <iostream>
#include "Mouse.h"

class MouseFactory
{
public:
    virtual Mouse* CreateMouse(void) = 0;
    virtual void DestroyMouse(Mouse* mouse) = 0;
};


#endif


