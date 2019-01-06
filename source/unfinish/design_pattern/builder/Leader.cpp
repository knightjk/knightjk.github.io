
#include "Leader.h"

using namespace std;


MainFrameAssembler* Leader::getHpMainFrameAssembler(void)
{
    HpAssemblerBuilder hpAssemblerBuilder;
    hpAssemblerBuilder.setInstallAlgorithm();
    return hpAssemblerBuilder.getMainFrameAssembler();
}

