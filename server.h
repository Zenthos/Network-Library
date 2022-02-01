// STD Console Header Files
#include <sdkddkver.h>
#include <conio.h>
#include <stdio.h>
#include <string>

// Socket Header Files
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>

// Windows threading library
#include <processthreadsapi.h>

// Function Prototypes
DWORD WINAPI ClientHandler(LPVOID arg);