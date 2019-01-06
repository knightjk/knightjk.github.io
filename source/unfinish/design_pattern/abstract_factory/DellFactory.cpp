
#include "DellFactory.h"

using namespace std;

Mouse* DellFactory::CreateMouse(void) 
{
    return new DellMouse;
}

KeyBoard* DellFactory::CreateKeyBoard(void) 
{
    return NULL;
}

void DellFactory::RecycleMouse(Mouse* mouse) 
{
    if(mouse != NULL)
        delete mouse;
}

void DellFactory::RecycleKeyBoard(KeyBoard* keyBoard) 
{
    cout<<"Dell do not product key board"<<endl;
}


