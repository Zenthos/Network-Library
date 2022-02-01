#pragma once

//Standard HEADER files
#include <sdkddkver.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>

// Windows threading library
#include <processthreadsapi.h>

// Function Prototypes
DWORD WINAPI ReceiveMessages(LPVOID arg);