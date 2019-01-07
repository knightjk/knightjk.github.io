
#include "AcerFactory.h"

using namespace std;

Mouse* AcerFactory::CreateMouse(void)
{
    return NULL;
}

KeyBoard* AcerFactory::CreateKeyBoard(void)
{
    return new AcerKeyBoard;
}

void AcerFactory::RecycleMouse(Mouse* mouse)
{
    cout<<"Acer do not product mouse"<<endl;
}

void AcerFactory::RecycleKeyBoard(KeyBoard* keyBoard)
{
    if(keyBoard != NULL)
        delete keyBoard;
}

