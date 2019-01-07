
#ifndef __DELLMOUSE_H__
#define __DELLMOUSE_H__

#include <iostream>
#include "Mouse.h"

class DellMouse: public Mouse
{
public:
    virtual void Click(void);
    virtual void RollWheel(void);
};


#endif

