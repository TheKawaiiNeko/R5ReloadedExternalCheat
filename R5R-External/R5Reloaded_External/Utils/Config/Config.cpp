#include "Config.h"

cfg g;

bool IsKeyDown(int VK)
{
    return (GetAsyncKeyState(VK) & 0x8000) != 0;
}