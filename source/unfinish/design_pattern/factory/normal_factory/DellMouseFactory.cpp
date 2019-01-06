
#include "DellMouseFactory.h"


using namespace std;

Mouse* DellMouseFactory::CreateMouse(void)
{
    return new DellMouse;
}
void DellMouseFactory::DestroyMouse(Mouse* mouse)
{
    if(mouse != NULL)
        delete mouse;
}

void DellMouseFactory::DestroyMouse(DellMouse* mouse)
{
    if(mouse != NULL)
        delete mouse;
}

