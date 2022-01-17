g++ dll_injector.cpp -o dll_injector.exe
g++ victim.cpp -o victim.exe
g++ -c dllmain.cpp -o dllmain.o
g++ -shared dllmain.o -o dllmain.dll
rm dllmain.o
