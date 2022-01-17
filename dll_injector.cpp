#include <iostream>
#include <windows.h>

bool inject_dll(const char* dll_path, int process_id)
{
    // Opens a handle to the desired process.
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);

    // Calculate the size of the path plus the null terminator.
    int dll_path_size = strlen(dll_path) + 1;

    // Allocates the needed memory to write the dll path.
    void* dll_allocated_memory =
        VirtualAllocEx(hProcess, NULL, dll_path_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    
    // Writes the dll path to the allocated memory.
    if (!WriteProcessMemory(
        hProcess, dll_allocated_memory, (LPVOID)dll_path, dll_path_size, NULL
    ))
    {
        std::cout << "Failed to write to allocated memory.\n";

        // Frees the allocated memory of our dll.
        VirtualFreeEx(hProcess, dll_allocated_memory, dll_path_size, MEM_RELEASE);
        
        return false;
    }

    // Creates a thread in the process that initializes the LoadLibraryA
    // with the address of our dll as argument.
    HANDLE hLoadLibraryThread =
        CreateRemoteThread( hProcess
                          , NULL
                          , NULL
                          , (LPTHREAD_START_ROUTINE)LoadLibraryA
                          , dll_allocated_memory
                          , NULL
                          , NULL
    );

    // Waits for the thread to load.
    WaitForSingleObject(hLoadLibraryThread, INFINITE);

    int exit_code;
    GetExitCodeThread(hLoadLibraryThread, (LPDWORD)&exit_code);

    if (exit_code)
    {
        std::cout << "Dll \"" << dll_path << "\" allocated at: " << dll_allocated_memory << ".\n";
        std::cout << "Awaiting input for detachment procedure.\n";
        std::cin.get();
    }

    std::cout << "Thread exited with code " << exit_code << ", finalizing the program.\n";
    std::cout << "Freeing the allocated memory.\n";

    // Closes the thread used to load the dll.
    CloseHandle(hLoadLibraryThread);

    // Frees the allocated memory of our dll.
    VirtualFreeEx(hProcess, dll_allocated_memory, dll_path_size, MEM_RELEASE);

    // Closes the process handle.
    CloseHandle(hProcess);

    return exit_code != 0;
}

int main(int argc, char** argv)
{
    const char* dll = argv[1];
    const int   pID = atoi(argv[2]);

    std::cout << "Injecting " << dll << " into process " << pID << ".\n";

    inject_dll(dll, pID);

    return 0;
}
