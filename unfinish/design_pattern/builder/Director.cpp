
#include "Director.h"

using namespace std;


MainFrameAssembler* Director::getHpMainFrameAssembler(void)
{
    HpAssemblerBuilder hpAssemblerBuilder;
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
    
    hpAssemblerBuilder.setInstallProcess(installProcess);

    return hpAssemblerBuilder.getMainFrameAssembler();

}


