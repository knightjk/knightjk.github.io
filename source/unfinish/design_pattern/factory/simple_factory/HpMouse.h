
#ifndef __HPMOUSE_H__
#define __HPMOUSE_H__
#include <iostream>
#include "Mouse.h"


class HpMouse: public Mouse
{
public:
    virtual void Click(void);
    virtual void RollWheel(void);
};

#endif

