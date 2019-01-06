
#include "HpAssemblerBuilder.h"


using namespace std;

HpAssemblerBuilder::HpAssemblerBuilder():AssemblerBuilder()
{
    this->pmainFrameAssembler = new HpMainFrameAssembler;
}

MainFrameAssembler* HpAssemblerBuilder::getMainFrameAssembler(void)    //写到这里有点工厂模式的味道了
{
    return this->pmainFrameAssembler;
}

void HpAssemblerBuilder::setInstallAlgorithm(void)
{
    list<InstallStep_t> installProcess;

    installProcess.clear();
    //install main board
    installProcess.push_back(INSTALL_MAIN_BOARD);
    //install memory
    installProcess.push_back(INSTALL_MEMORY);
    //install cpu
    installProcess.push_back(INSTALL_CPU);
    //install gpu
    installProcess.push_back(INSTALL_GPU);
    //install hdd
    installProcess.push_back(INSTALL_HDD);

    this->pmainFrameAssembler->setInstallProcess(installProcess);
}

void HpAssemblerBuilder::setInstallProcess(list<InstallStep_t> installProcess)
{
    this->pmainFrameAssembler->setInstallProcess(installProcess);
}
