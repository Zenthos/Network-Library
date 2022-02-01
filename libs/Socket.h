#pragma once
#include "Network.h"
#include "Endpoint.h"

namespace Network
{
  enum SockOpt { TCP_NoDelay, SO_ReuseAddr };

  class Socket
  {
    // Socket Constructor
    public:
      Socket(	IPVersion ipversion = IPVersion::IPv4,
				      SocketHandle handle = INVALID_SOCKET);
    // Public Functions
    public:
      Result Init();
      Result Close();
      Result Bind(Endpoint endpoint);
      Result Listen(Endpoint endpoint, int backlog = 5);
      Result Accept(Socket& outSocket);
      Result Connect(Endpoint endpoint);
      Result Send(const void* data, int bytesToSend, int& bytesSent);
      Result Recv(const void* destination, int bufferSize, int& bytesReceived);
      Result SendAll(const void* data, int bytesToSend);

      // Getters and Setters
      SocketHandle GetHandle();
      IPVersion GetIPVersion();
      
    // Private Variables
    private:
      IPVersion ipversion = IPVersion::IPv4;
      SocketHandle handle = INVALID_SOCKET;
    // Private Functions
    private:
      Result SetSocketOption(SockOpt option, BOOL value);
  };
}