
#ifndef __SIMPLEMOUSEFACTORY_H__
#define __SIMPLEMOUSEFACTORY_H__

#include <iostream>
#include "Mouse.h"
#include "HpMouse.h"
#include "DellMouse.h"

typedef enum{
    HpMouseType = 0,
    DellMouseType = 1,
}MouseType_t;

class SimpleMouseFactory
{
public:
    Mouse* CreateMouse(MouseType_t mouseType);
    void DestroyMouse(Mouse* mouse);
};


#endif
