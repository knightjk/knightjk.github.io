
#ifndef __HPMOUSEFACTORY_H__
#define __HPMOUSEFACTORY_H__

#include <iostream>
#include "MouseFactory.h"
#include "HpMouse.h"

class HpMouseFactory: public MouseFactory
{
public:
    Mouse* CreateMouse(void);
    void DestroyMouse(Mouse* mouse);
};

#endif
