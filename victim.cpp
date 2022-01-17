#include <iostream>
#include <windows.h>

int main(void)
{
    int pID = GetCurrentProcessId();
    
    while (true)
    {
        std::cout << "Hello! My current process ID is: " << pID << ".\n";
        Sleep(1000);
    }
}
