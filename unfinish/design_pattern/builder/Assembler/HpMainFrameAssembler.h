
#ifndef __HPMAINFRAMEASSEMBLER_H__
#define __HPMAINFRAMEASSEMBLER_H__

#include "MainFrameAssembler.h"

class HpMainFrameAssembler: public MainFrameAssembler
{
public:
    virtual void installMainBoard(void) ;
    virtual void installMemory(void) ;
    virtual void installCpu(void) ;
    virtual void installGpu(void) ;
    virtual void installHdd(void) ;
};

#endif
