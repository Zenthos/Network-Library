#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <stdio.h>

namespace Network
{
  typedef SOCKET SocketHandle;
  enum IPVersion { Unknown, IPv4, IPv6 };
  enum Result { S_Ok, E_GenericError };

  class WinSockHandler
  {
    public:
      static bool Initialize();
      static bool Shutdown();
  };
}
