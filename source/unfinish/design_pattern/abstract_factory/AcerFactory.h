
#ifndef __ACERFACTORY_H__
#define __ACERFACTORY_H__

#include "PcFactory.h"
#include "AcerKeyBoard.h"

class AcerFactory: public PcFactory
{
public:
    virtual Mouse* CreateMouse(void);
    virtual KeyBoard* CreateKeyBoard(void);

    virtual void RecycleMouse(Mouse* mouse);
    virtual void RecycleKeyBoard(KeyBoard* keyBoard);
};

#endif

