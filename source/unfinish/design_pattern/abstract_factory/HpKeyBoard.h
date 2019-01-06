
#ifndef __HPKEYBOARD_H__
#define __HPKEYBOARD_H__

#include <iostream>
#include "KeyBoard.h"

class HpKeyBoard: public KeyBoard
{
public:
    virtual void Typing(std::string str);
};


#endif 

