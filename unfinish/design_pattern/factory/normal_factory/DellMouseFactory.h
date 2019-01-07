
#ifndef __DELLMOUSEFACTORY_H__
#define __DELLMOUSEFACTORY_H__

#include "MouseFactory.h"
#include "DellMouse.h"

class DellMouseFactory: public MouseFactory
{
public:
    Mouse* CreateMouse(void);
    void DestroyMouse(Mouse* mouse);
    void DestroyMouse(DellMouse* mouse);
};

#endif

