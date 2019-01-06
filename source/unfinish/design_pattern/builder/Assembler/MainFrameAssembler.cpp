
#include "MainFrameAssembler.h"

using namespace std;

void MainFrameAssembler::startInstall(void)
{
    for(std::list<InstallStep_t>::iterator iter = this->installProcess.begin();
        iter != this->installProcess.end();iter++)
    {
        switch((int)(*iter))
        {
            case INSTALL_MAIN_BOARD:
                this->installMainBoard();
                break;
            case INSTALL_MEMORY:
                this->installMemory();
                break;
            case INSTALL_CPU:
                this->installCpu();
                break;
            case INSTALL_GPU:
                this->installGpu();
                break;
            case INSTALL_HDD:
                this->installHdd();
                break;
            default: 
                break;
        }
    }
}

void MainFrameAssembler::setInstallProcess(std::list<InstallStep_t> installProcess)
{
    this->installProcess.clear();
    this->installProcess.assign(installProcess.begin(),installProcess.end());
}








