
#ifndef __ASSEMBLERBUILDER_H__
#define __ASSEMBLERBUILDER_H__

#include "../Assembler/MainFrameAssembler.h"

class AssemblerBuilder
{
public:
    virtual MainFrameAssembler* getMainFrameAssembler(void) = 0;    //写到这里有点工厂模式的味道了
    virtual void setInstallAlgorithm(void) = 0;//leader会选择的方法
    virtual void setInstallProcess(std::list<InstallStep_t> installProcess) = 0;// director会选择的方法

protected:
    MainFrameAssembler* pmainFrameAssembler;
};

#endif

