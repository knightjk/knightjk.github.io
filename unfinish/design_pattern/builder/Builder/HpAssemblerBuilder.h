
#ifndef __HPASSEMBLERBUILDER_H__
#define __HPASSEMBLERBUILDER_H__

#include "AssemblerBuilder.h"
#include "../Assembler/HpMainFrameAssembler.h"

class HpAssemblerBuilder :public AssemblerBuilder
{
public:
    HpAssemblerBuilder();
    virtual MainFrameAssembler* getMainFrameAssembler(void);    //写到这里有点工厂模式的味道了
    virtual void setInstallAlgorithm(void);
    virtual void setInstallProcess(std::list<InstallStep_t> installProcess);// director会选择的方法
};

#endif

