#include "server.h"
#include "libs/Network.h"

int main() {
  WSAData winSockData;
  WORD DLLVERSION = MAKEWORD(2, 1);
  WSAStartup(DLLVERSION, &winSockData);

  // Create and initialize the server address struct
  sockaddr_in address;
  int addressSize = sizeof(address);
  address.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
  address.sin_family = AF_INET;
  address.sin_port = htons(50231);

  // Create the server socket, bind it to the port, and begin listening for connections
  SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
  bind(listener, (sockaddr*)&address, addressSize);
  listen(listener, SOMAXCONN);

  // Forever loop to continuously accept connections
  printf("[S]: Server waiting for incoming connection...\n");
  while (true)
  {
    // Perform an action whenever a valid connection is established
    SOCKET client = accept(listener, (sockaddr*)&address, &addressSize);

    if (client) {
      printf("[S]: A client has connected!\n");

      // Send greeting message
      char greeting[] = "Welcome! You have connected to my c++ socket server!";
      // Send string and size of string + null terminator
      send(client, greeting, strlen(greeting) + 1, 0); 

      DWORD threadId;
      HANDLE thread = CreateThread( 
        NULL,                   // default security attributes
        0,                      // use default stack size  
        ClientHandler,          // thread function name
        (LPVOID)client,         // argument to thread function 
        0,                      // use default creation flags 
        &threadId               // returns the thread identifier 
      );

      if (thread == NULL) 
      {
        printf("[S]: Could not generate a separate thread for the client.\n");
        closesocket(listener);
        WSACleanup();
        return EXIT_FAILURE;
      }
    }
  }
}

DWORD WINAPI ClientHandler(LPVOID arg) {
  SOCKET client = (SOCKET)arg;
  char buffer[512];

  // Receive data from client
  while (true)
  {
    int size = recv(client, buffer, sizeof(buffer), 0);

    // Check if client is disconnecting
    if (strcmp(buffer, "exit") == 0 || size == -1)
    {
      printf("[S]: A client has disconnected.\n");
      break;
    }

    send(client, buffer, strlen(buffer) + 1, 0); 
  }

  return 0;
}