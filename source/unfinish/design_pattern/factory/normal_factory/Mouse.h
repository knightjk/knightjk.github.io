#ifndef __MOUSE_H__
#define __MOUSE_H__

#include <iostream>

class Mouse
{
public:
    virtual void Click(void) = 0;
    virtual void RollWheel(void) = 0;
    virtual ~Mouse(){};
private:

};

#endif
