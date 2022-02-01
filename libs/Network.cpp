#include "Network.h"

bool Network::WinSockHandler::Initialize()
{
  WSAData data;
  int startup = WSAStartup(MAKEWORD(2, 2), &data);

  // Could not find a usable winsock32.dll on the system
  if (startup != 0)
  {
    printf("WSAStartup failed with error: %d\n", startup);
    return false;
  }

  // Confirm that the winsock32 dll matches the one that was requested
  if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2)
  {
    printf("Could not find a usable version of the winsock32.dll\n");
    WSACleanup();
    return false;
  }

  return true;
}

bool Network::WinSockHandler::Shutdown()
{
  WSACleanup();
  return true;
}