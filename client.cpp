//TCP CLIENT source file
#include "client.h"
#include "libs\Network.h"

int main() {
  WSAData winSockData;
  WORD DLLVERSION = MAKEWORD(2, 1);
  WSAStartup(DLLVERSION, &winSockData);

  // Initialize socket
  SOCKET clientSocket;

  // Create the socket, and ensure its valid
  if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    printf("[C]: Error creating client socket, error code: %d\n", WSAGetLastError());
    WSACleanup();
    return EXIT_FAILURE;
  }
  
  // Initialize server address to connect to
  sockaddr_in address;
  int addressSize = sizeof(address);
  address.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
  address.sin_family = AF_INET;
  address.sin_port = htons(50231);

  // Ensure a valid connection is established
  if (connect(clientSocket, (sockaddr *)&address, addressSize) == SOCKET_ERROR)
  {
    printf("[C]: The client failed to connect to 127.0.0.1\n");
    closesocket(clientSocket);
    WSACleanup();
    return EXIT_FAILURE;
  }

  DWORD threadId;
  HANDLE thread = CreateThread(
    NULL,                   // default security attributes
    0,                      // use default stack size  
    ReceiveMessages,        // thread function name
    (LPVOID)clientSocket,   // argument to thread function 
    0,                      // use default creation flags 
    &threadId               // returns the thread identifier 
  );

  if (thread == NULL) 
  {
    printf("[S]: Could not generate a separate thread for the client.\n");
    closesocket(clientSocket);
    WSACleanup();
    return EXIT_FAILURE;
  }

  while (true)
  {
    std::string input;
    std::getline(std::cin, input);

    // Send string and size of string + null terminator
    send(clientSocket, input.c_str(), strlen(input.c_str()) + 1, 0); 
    
    // Break loop once user inputs the exit keyword
    if (strcmp(input.c_str(), "exit") == 0)
    {
      printf("Safely Exiting...\n");
      break;
    }
  }

  // Cleanup
  closesocket(clientSocket);
  WSACleanup();
  return EXIT_SUCCESS;
}

DWORD WINAPI ReceiveMessages(LPVOID arg) {
  SOCKET clientSocket = (SOCKET)arg;
  char buffer[512];

  // Receive data from server
  while (true)
  {
    int size = recv(clientSocket, buffer, sizeof(buffer), 0);

    // A valid message was received
    if (size != -1)
    {
      printf("[S]: %s\n", buffer);
    }
  }

  return 0;
}
