#include "libs\Network.h"
#include "libs\Socket.h"
#include "libs\Endpoint.h"
#include <iostream>
#include <processthreadsapi.h>
#include <time.h>

DWORD WINAPI server(LPVOID arg);
DWORD WINAPI client(LPVOID arg);

int main()
{
	HANDLE serverThread = CreateThread( 
		NULL,     // default security attributes
		0,        // use default stack size  
		server,   // thread function name
		NULL,     // argument to thread function 
		0,        // use default creation flags 
		nullptr		// returns the thread identifier 
	);

	Sleep(1000);

	HANDLE clientThread = CreateThread( 
		NULL,     // default security attributes
		0,        // use default stack size  
		client,   // thread function name
		NULL,     // argument to thread function 
		0,        // use default creation flags 
		nullptr		// returns the thread identifier 
	);

	// Windows equivalent of Thread join()
	WaitForSingleObject(serverThread, INFINITE);
	WaitForSingleObject(clientThread, INFINITE);
	return 0;
};

DWORD WINAPI server(LPVOID arg)
{
	if (Network::WinSockHandler::Initialize())
	{
		std::cout << "[S]: Server winsock api successfully initialized." << std::endl;

		Network::Socket socket;
		if (socket.Init() == Network::Result::S_Ok)
		{
			std::cout << "[S]: Socket successfully created." << std::endl;

			if (socket.Listen(Network::Endpoint("127.0.0.1", 8080)) == Network::Result::S_Ok)
			{
				std::cout << "[S]: Socket successfully listening on port: " << 8080 << std::endl;

				Network::Socket newConnection;
				if (socket.Accept(newConnection) == Network::Result::S_Ok)
				{
					std::cout << "[S]: Connection was accepted!" << std::endl;

					char buffer[256];
					int bytesReceived = 0;
					int result = Network::Result::S_Ok;

					while (result == Network::Result::S_Ok)
					{
						result = newConnection.Recv(buffer, 256, bytesReceived);

						if (result != Network::Result::S_Ok)
							break;

						std::cout << "[S]: Server has a received a message from a client!" << std::endl;
						std::cout << buffer << std::endl;
					}

					newConnection.Close();
				}
			}

			socket.Close();
		}
		else
		{
			std::cerr << "[S]: Socket failed to create." << std::endl;
		}
	}

	Network::WinSockHandler::Shutdown();
	return 0;
};

DWORD WINAPI client(LPVOID arg) 
{
	if (Network::WinSockHandler::Initialize())
	{
		std::cout << "[C]: Client winsock api successfully initialized." << std::endl;

		Network::Socket socket;
		if (socket.Init() == Network::Result::S_Ok)
		{
			std::cout << "[C]: Socket successfully created." << std::endl;

			if (socket.Connect(Network::Endpoint("127.0.0.1", 8080)) == Network::Result::S_Ok)
			{
				std::cout << "[C]: Socket successfully connected to the server." << std::endl;

				char buffer[] = "Hello, this is a message the client is sending to the server.";
				int result = Network::Result::S_Ok;

				while (result == Network::Result::S_Ok)
				{
					std::cout << "[C]: Attempting to send chunk of data..." << std::endl;
					result = socket.SendAll(buffer, strlen(buffer) + 1);
					std::cout << "[C]: Client has successfully sent " << strlen(buffer) + 1 << " bytes to the server" << std::endl;
					Sleep(2000);
				}
			}

			socket.Close();
		}
		else
		{
			std::cerr << "[C]: Socket failed to create." << std::endl;
		}
	}

	Network::WinSockHandler::Shutdown();
	return 0;
};