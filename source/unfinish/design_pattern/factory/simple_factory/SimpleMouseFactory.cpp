#include "SimpleMouseFactory.h"

using namespace std;

Mouse* SimpleMouseFactory::CreateMouse(MouseType_t mouseType)
{
    switch(mouseType)
    {
        case HpMouseType:
            cout<<"create HP mouse"<<endl;
            return new HpMouse;
            break;
        case DellMouseType:
            cout<<"create Dell mouse"<<endl;
            return new DellMouse;
            break;
        default:
            return NULL;
    }
    return NULL;
}

void SimpleMouseFactory::DestroyMouse(Mouse* mouse)
{
    if(mouse != NULL)
        delete mouse;
}

