
#ifndef __HPFACTORY_H__
#define __HPFACTORY_H__

#include "PcFactory.h"
#include "HpMouse.h"
#include "HpKeyBoard.h"

class HpFactory: public PcFactory
{
public:
    virtual Mouse* CreateMouse(void) ;
    virtual KeyBoard* CreateKeyBoard(void) ;

    virtual void RecycleMouse(Mouse* mouse) ;
    virtual void RecycleKeyBoard(KeyBoard* keyBoard) ;
};


#endif
