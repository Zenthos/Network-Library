#include "Socket.h"
#include <assert.h>

namespace Network
{
  Socket::Socket(IPVersion ipversion, SocketHandle handle):ipversion(ipversion), handle(handle)
  {
    assert(ipversion == IPVersion::IPv4);
  }

  Result Socket::Init()
  {
    assert(ipversion == IPVersion::IPv4);

    if (handle != INVALID_SOCKET)
      return Result::E_GenericError;

    handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (handle == INVALID_SOCKET)
    {
      int error = WSAGetLastError();
      printf("Error initializing socket: %d\n", error);
      return Result::E_GenericError;
    }

    if (SetSocketOption(SockOpt::TCP_NoDelay, TRUE) != Result::S_Ok)
      return Result::E_GenericError;

    if (SetSocketOption(SockOpt::SO_ReuseAddr, TRUE) != Result::S_Ok)
      return Result::E_GenericError;

    return Result::S_Ok;
  }

  Result Socket::Close()
  {
    if (handle == INVALID_SOCKET)
    {
      printf("Error: Attempting to close an invalid socket.");
      return Result::E_GenericError;
    }

    int result = closesocket(handle);
    if (result != 0)
    {
      int error = WSAGetLastError();
      printf("Error closing socket: %d\n", error);
      return Result::E_GenericError;
    }

    handle = INVALID_SOCKET;
    return Result::S_Ok;
  }

  Result Socket::SetSocketOption(SockOpt option, BOOL value)
  {
    if (handle == INVALID_SOCKET)
    {
      printf("Error: Attempting to set an option for an invalid socket.");
      return Result::E_GenericError;
    }

    int result = 0;

    switch(option)
    {
      case SockOpt::TCP_NoDelay: // Disable nagle's algorithm
        result = setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
        break;
      case SockOpt::SO_ReuseAddr: // Reuse addresses that are already in use
        result = setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, (const char*)&value, sizeof(value));
        break;
      default:
        return Result::E_GenericError;
    }

    if (result != 0)
    {
      int error = WSAGetLastError();
      printf("Error setting socket option: %d\n", error);
      return Result::E_GenericError;
    }

    return Result::S_Ok;
  }

  Result Socket::Bind(Endpoint endpoint)
  {
    sockaddr_in addr = endpoint.GetSockAddrIPv4();

    int result = bind(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
    if (result != 0)
    {
      int error = WSAGetLastError();
      printf("Error binding socket to endpoint. Error Code: %d\n", error);
      return Result::E_GenericError;
    }

    return Result::S_Ok;
  }

  Result Socket::Listen(Endpoint endpoint, int backlog)
  {
    if (Bind(endpoint) != Result::S_Ok)
    {
      return Result::E_GenericError;
    }

    int result = listen(handle, backlog);
    if (result !=  0)
    {
      int error = WSAGetLastError();
      printf("Error when attempting to make socket listen. Error Code: %d\n", error);
      return Result::E_GenericError;    
    }

    return Result::S_Ok;
  }

  Result Socket::Accept(Socket& outSocket)
  {
    sockaddr_in addr = {};
    int len = sizeof(sockaddr_in);

    SocketHandle acceptedConnectionHandle = accept(handle, (sockaddr*)(&addr), &len);
    if (acceptedConnectionHandle == INVALID_SOCKET)
    {
      int error = WSAGetLastError();
      printf("Error when attempting to accept connection. Error Code: %d\n", error);
      return Result::E_GenericError;    
    }

    // Otherwise print out connection information
    Endpoint endpoint((sockaddr*)(&addr));
    endpoint.Print();
    
    // Create a new socket object for the incoming connection
    outSocket = Socket(IPVersion::IPv4, acceptedConnectionHandle);
    return Result::S_Ok;
  }

  Result Socket::Connect(Endpoint endpoint)
  {
    sockaddr_in addr = endpoint.GetSockAddrIPv4();
    int result = connect(handle, (sockaddr*)(&addr), sizeof(addr));
    if (result !=  0)
    {
      int error = WSAGetLastError();
      printf("Error when attempting to connect to endpoint. Error Code: %d\n", error);
      return Result::E_GenericError;    
    }

    return Result::S_Ok;
  }

  Result Socket::Send(const void* data, int bytesToSend, int& bytesSent)
  {
    bytesSent = send(handle, (const char*)data, bytesToSend, 0);
    if (bytesSent == SOCKET_ERROR)
    {
      int error = WSAGetLastError();
      printf("Error sending packet of data. Error Code: %d\n", error);
      return Result::E_GenericError;    
    }

    return Result::S_Ok;
  }

  Result Socket::Recv(const void* destination, int bufferSize, int& bytesReceived)
  {
    bytesReceived = recv(handle, (char*)destination, bufferSize, 0);

    // Graceful Close
    if (bytesReceived == 0)
    {
      return Result::E_GenericError;
    }

    // Error receiving data
    if (bytesReceived == SOCKET_ERROR)
    {
      int error = WSAGetLastError();
      printf("Error receiving packet of data. Error Code: %d\n", error);
      return Result::E_GenericError;  
    }

    return Result::S_Ok;
  }

  Result Socket::SendAll(const void* data, int bytesToSend)
  {
		int totalBytesSent = 0;

		while (totalBytesSent < bytesToSend)
		{
			int bytesRemaining = bytesToSend - totalBytesSent;
			int bytesSent = 0;

			char * bufferOffset = (char*)data + totalBytesSent;
			Result result = Send(bufferOffset, bytesRemaining, bytesSent);

			if (result != Result::S_Ok)
				return Result::E_GenericError;

			totalBytesSent += bytesSent;
		}

		return Result::S_Ok;
  }

  // Getters
  SocketHandle Socket::GetHandle() { return handle; }
  IPVersion Socket::GetIPVersion() { return ipversion; }
  
  // Setters
}

// https://www.youtube.com/watch?v=rp6WT0HWLRo&list=PLcacUGyBsOIA1YUqvKd7p8uvsG_FCt7-U&index=7