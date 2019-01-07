
#ifndef __MAINFRAMEASSEMBLER_H__
#define __MAINFRAMEASSEMBLER_H__

#include <iostream>
#include <list>

typedef enum{
    INSTALL_MAIN_BOARD,
    INSTALL_MEMORY,
    INSTALL_CPU,
    INSTALL_GPU,
    INSTALL_HDD,
}InstallStep_t;


class MainFrameAssembler
{
public:
    void startInstall(void);//这是装配的不变流程部分,是不用客户修改的部分
    void setInstallProcess(std::list<InstallStep_t> installProcess);
protected:
    virtual void installMainBoard(void) = 0;
    virtual void installMemory(void) = 0;
    virtual void installCpu(void) = 0;
    virtual void installGpu(void) = 0;
    virtual void installHdd(void) = 0;
private:
    std::list<InstallStep_t> installProcess;
};


#endif

