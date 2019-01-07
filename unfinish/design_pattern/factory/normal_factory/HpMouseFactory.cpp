#include "HpMouseFactory.h"

using namespace std;

Mouse* HpMouseFactory::CreateMouse(void)
{
    return new HpMouse;
}
void HpMouseFactory::DestroyMouse(Mouse* mouse)
{
    if(mouse != NULL)
        delete mouse;
}
