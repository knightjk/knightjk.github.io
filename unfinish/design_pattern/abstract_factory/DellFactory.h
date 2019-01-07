
#ifndef __DELLFACTORY_H__
#define __DELLFACTORY_H__

#include <iostream>
#include "PcFactory.h"
#include "DellMouse.h"

class DellFactory: public PcFactory
{
public:
    virtual Mouse* CreateMouse(void) ;
    virtual KeyBoard* CreateKeyBoard(void) ;

    virtual void RecycleMouse(Mouse* mouse) ;
    virtual void RecycleKeyBoard(KeyBoard* keyBoard) ;
};



#endif

