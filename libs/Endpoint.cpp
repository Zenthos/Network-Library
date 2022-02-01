#include "Endpoint.h"
#include <assert.h>

namespace Network
{
  Endpoint::Endpoint(const char* ip, unsigned short port)
  {
    this->port = port;

    // Parse ipv4 if it's in x.x.x.x format
    in_addr addr;
    int result = inet_pton(AF_INET, ip, &addr);

    if (result == 1)
    {
      if (addr.S_un.S_addr != INADDR_NONE)
      {
        ip_string = ip;
        hostname = ip;
        ip_bytes.resize(sizeof(ULONG));
        memcpy(&ip_bytes[0], &addr.S_un.S_addr, sizeof(ULONG));
        ipversion = IPVersion::IPv4;
        return;
      }
    }

    // Parse ipv4 if it's in www.HOSTNAME.com format
    addrinfo hints = {};
    hints.ai_family = AF_INET; //ipv4 addresses only
    addrinfo* hostinfo;
    result = getaddrinfo(ip, NULL, &hints, &hostinfo);

    if (result == 0)
    {
			sockaddr_in * host_addr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);

      // set ip string
			ip_string.resize(16);
			inet_ntop(AF_INET, &host_addr->sin_addr, &ip_string[0], 16);
      
      // set ip bytes
			ULONG ip_long = host_addr->sin_addr.S_un.S_addr;
			ip_bytes.resize(sizeof(ULONG)); 
			memcpy(&ip_bytes[0], &ip_long, sizeof(ULONG));

      // set other endpoint info
			hostname = ip;
			ipversion = IPVersion::IPv4;
 
      //memory cleanup from getaddrinfo call
			freeaddrinfo(hostinfo);
			return;
    }
  } 

  Endpoint::Endpoint(sockaddr* addr)
  {
    assert(addr->sa_family == AF_INET);
    sockaddr_in* addr_v4 = reinterpret_cast<sockaddr_in*>(addr);

    // set ip bytes
    ip_bytes.resize(sizeof(ULONG));
    memcpy(&ip_bytes[0], &addr_v4->sin_addr, sizeof(ULONG));

    // set ip string
    ip_string.resize(16);
    inet_ntop(AF_INET, &addr_v4->sin_addr, &ip_string[0], 16);

    hostname = ip_string;
    port = ntohs(addr_v4->sin_port);
    ipversion = IPVersion::IPv4;
  }

  sockaddr_in Endpoint::GetSockAddrIPv4()
  {
    assert(ipversion == IPVersion::IPv4);
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    memcpy(&addr.sin_addr, &ip_bytes[0], sizeof(ULONG));
    addr.sin_port = htons(port);
    return addr;
  }

  sockaddr_in6 Endpoint::GetSockAddrIPv6()
  {
    assert(ipversion == IPVersion::IPv6);
    sockaddr_in6 addr = {};

    return addr;
  }

  void Endpoint::Print()
  {
    switch (ipversion)
		{
		case IPVersion::IPv4:
			std::cout << "IP Version: IPv4" << std::endl;
			break;
		case IPVersion::IPv6:
			std::cout << "IP Version: IPv6" << std::endl;
			break;
		default:
			std::cout << "IP Version: Unknown" << std::endl;
		}
		std::cout << "Hostname: " << hostname << std::endl;
		std::cout << "IP: " << ip_string << std::endl;
		std::cout << "Port: " << port << std::endl;
		std::cout << "IP bytes..." << std::endl;
		for (int i = 0; i < (int)ip_bytes.size(); i++)
		{
			std::cout << (int)ip_bytes[i];

      if (i != (int)ip_bytes.size() -1)
        std::cout << ", ";
		}
    std::cout << std::endl;
  }

  // Basic Getters
  IPVersion Endpoint::GetIPVersion() { return ipversion; }
  std::vector<uint8_t> Endpoint::GetIPBytes() { return ip_bytes; }
  std::string Endpoint::GetHostName() { return hostname; }
  std::string Endpoint::GetIPString() { return ip_string; }
  unsigned short Endpoint::GetPort() { return port; }

}