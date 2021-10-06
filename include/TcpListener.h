#pragma once

#include <WS2tcpip.h>					// Header file for  WinSock functions

#include <string>

#pragma comment(lib, "ws2_32.lib")		// WinSock library file

#define MAX_BUFFER_SIZE 4096

// Forward declaration
class TcpListener;

// Callback to data received
using MessageReceivedHandler = void(*)(TcpListener* listener, int socketId, std::string msg);

class TcpListener
{
private:
	std::string				m_IpAddress;
	int						m_port;
	MessageReceivedHandler	MessageReceived;

public:
	TcpListener(std::string ipAddress, int port, MessageReceivedHandler handler);

	~TcpListener();

	// Initialize WinSock
	bool Init();

	// main processing loop
	void Run();

	// Send a message to the specified client
	void Send(int clientSocket, std::string msg);

	// Cleanup
	void Cleanup();

private:
	// Create a socket
	SOCKET CreateSocket();

	// Wait for a connection
	SOCKET WaitForConnection(SOCKET listening);
};