#include "TcpListener.h"

#include <iostream>

TcpListener::TcpListener(std::string ipAddress, int port, MessageReceivedHandler handler)
	: m_IpAddress{ ipAddress }, m_port{ port }, MessageReceived{ handler }
{
}

TcpListener::~TcpListener()
{
	Cleanup();
}

// Initialize WinSock
bool TcpListener::Init()
{
	WSAData data;
	WORD ver{ MAKEWORD(2, 2) };

	int wsInit({ WSAStartup(ver, &data) });
	if (wsInit != 0)
	{
		std::cerr << "Can't initialize WinSock! Error: " << wsInit << '\n';
	}

	return wsInit == 0;
}

// main processing loop
void TcpListener::Run()
{
	// Create a listening socket
	SOCKET listening{ CreateSocket() };
	if (listening == INVALID_SOCKET)
	{
		return;
	}

	// Wait for a connection
	SOCKET client{ WaitForConnection(listening) };
	if (client == INVALID_SOCKET)
	{
		return;
	}

	// Loop receive / send
	closesocket(listening);

	char buf[MAX_BUFFER_SIZE];
	while (true)
	{
		int bytesReceived{ recv(client, buf, MAX_BUFFER_SIZE, 0) };
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Can't read from client! Error: " << WSAGetLastError() << '\n';
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected!\n";
			break;
		}

		if (MessageReceived == NULL)
		{
			std::cerr << "No MessageReceivedHandler found! Quitting ... \n";
			break;
		}

		MessageReceived(this, client, std::string(buf, 0, bytesReceived));
	}

	closesocket(client);
}

// Send a message to the specified client
void TcpListener::Send(int clientSocket, std::string msg)
{
	send(clientSocket, msg.c_str(), msg.size(), 0);
}

// Cleanup
void TcpListener::Cleanup()
{
	WSACleanup();
}

// Create a socket
SOCKET TcpListener::CreateSocket()
{
	SOCKET listening{ socket(AF_INET, SOCK_STREAM, 0) };
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket! Error: " << WSAGetLastError() << '\n';
		return INVALID_SOCKET;
	}

	sockaddr_in hint{};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_port);
	inet_pton(AF_INET, m_IpAddress.c_str(), &hint.sin_addr);

	int bindOk{ bind(listening, (sockaddr*)&hint, sizeof(hint)) };
	if (bindOk == SOCKET_ERROR)
	{
		std::cerr << "Can't bind socket! Error: " << WSAGetLastError() << '\n';
		closesocket(listening);
		return INVALID_SOCKET;
	}

	int listenOk{ listen(listening, SOMAXCONN) };
	if (listenOk == SOCKET_ERROR)
	{
		std::cerr << "Can't listen on socket! Error: " << WSAGetLastError() << '\n';
		closesocket(listening);
		return INVALID_SOCKET;
	}

	return listening;
}

// Wait for a connection
SOCKET TcpListener::WaitForConnection(SOCKET listening)
{
	SOCKET client{ accept(listening, NULL, NULL) };
	if (client == INVALID_SOCKET)
	{
		std::cerr << "Can't accept client! Error: " << WSAGetLastError() << '\n';
	}

	return client;
}