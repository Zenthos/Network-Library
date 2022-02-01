#pragma once
#include "Network.h"
#include <WS2tcpip.h>
#include <iostream>
#include <vector>

namespace Network
{
  class Endpoint
  {
    // Endpoint Constructors
    public:
      Endpoint(const char* ip, unsigned short port);
      Endpoint(sockaddr* addr);
    // Public Functions
    public:
      void Print();
      // Getters
      IPVersion GetIPVersion();
      std::vector<uint8_t> GetIPBytes();
      std::string GetHostName();
      std::string GetIPString();
      unsigned short GetPort();
      sockaddr_in GetSockAddrIPv4();
      sockaddr_in6 GetSockAddrIPv6();

    // Private Variables
    private:
      IPVersion ipversion = IPVersion::IPv4;
      std::string hostname = "";
      std::string ip_string = "";
      std::vector<uint8_t> ip_bytes;
      unsigned short port = 0;
  };
}