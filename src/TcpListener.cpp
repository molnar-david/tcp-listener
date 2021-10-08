#include "TcpListener.h"

#include <iostream>
#include <string>

#define MAX_BUFFER_SIZE 4096

TcpListener::TcpListener(const char* ipAddress, int port)
	: m_ipAddress(ipAddress), m_port(port), m_running(false)
{
}

// Initialize the listener
int TcpListener::init()
{
	// Initialize WinSock
	WSADATA wsData{};
	WORD ver{ MAKEWORD(2, 2) };

	int ret{ WSAStartup(ver, &wsData) };
	if (ret != 0)
	{
		return ret;
	}

	// Create a socket
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}

	// Bind the ip address and port to a socke
	sockaddr_in hint{};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_port);			// host to network short
	inet_pton(AF_INET, m_ipAddress, &hint.sin_addr);

	ret = bind(m_socket, (sockaddr*)&hint, sizeof(hint));
	if (ret == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	// Tell WinSock the socket is for listening
	ret = listen(m_socket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	// Create the master file descriptor set and zero it
	FD_ZERO(&m_master);

	// Add our listening socket to the set
	FD_SET(m_socket, &m_master);

	return 0;
}

// Run the listener
int TcpListener::run()
{
	m_running = true;
	while (m_running)
	{
		// The call to select is destructive - the copy will only contain the sockets
		// that are accepting inbound connection requests or messages, so we'll need a copy!
		fd_set copy{ m_master };

		int socketCount{ select(0, &copy, nullptr, nullptr, nullptr) };
		if (socketCount == SOCKET_ERROR)
		{
			break;
		}

		for (int i{ 0 }; i < socketCount && m_running; ++i)
		{
			SOCKET sock{ copy.fd_array[i] };

			// Is it an inbound communication?
			if (sock == m_socket)
			{
				// Accept a new connection
				SOCKET client{ accept(m_socket, nullptr, nullptr) };
				if (client == INVALID_SOCKET)
				{
					continue;
				}

				// Add the new connection to the list of connected clients
				FD_SET(client, &m_master);

				// Send a welcome message to the client
				onClientConnected(client);
			}
			else	// It's an inbound message
			{
				// Receive message
				char buf[MAX_BUFFER_SIZE]{};
				int bytesReceived{ recv(sock, buf, MAX_BUFFER_SIZE, 0) };

				// Client disconnects
				if (bytesReceived <= 0)
				{
					onClientDisconnected(sock);

					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &m_master);
					continue;
				}

				onMessageReceived(sock, buf, bytesReceived);
			}
		}
	}

	shutdown();

	return 0;
}

// Shut the listener down
void TcpListener::shutdown()
{
	// Close the listening socket
	closesocket(m_socket);
	FD_CLR(m_socket, &m_master);
	std::cout << "Server shut down\n";

	std::string msg{ "Server is shutting down. Goodbye ...\r\n" };

	// Close open client sockets
	while (m_master.fd_count > 0)
	{
		SOCKET sock{ m_master.fd_array[0] };

		// Inform client about the server shutting down
		sendToClient(sock, msg.c_str(), msg.size());

		// Close the client socket, remove it from master set
		closesocket(sock);
		FD_CLR(sock, &m_master);
	}

	// Cleanup WinSock
	WSACleanup();
}

// Handler for client connections
void TcpListener::onClientConnected(int clientSocket)
{

}

// Handler for client disconnections
void TcpListener::onClientDisconnected(int clientSocket)
{

}

// Handler for when a message is received from the client
void TcpListener::onMessageReceived(int clientSocket, const char* msg, int length)
{
	// Is the message a command?
	if (msg[0] == '\\')
	{
		std::string cmd{ std::string(msg, length) };

		// Quit the server
		if (cmd == "\\quit")
		{
			setRunning(false);
		}

		// Unknown command - continue
	}
}

// Send a message to a client
void TcpListener::sendToClient(int clientSocket, const char* msg, int length)
{
	send(clientSocket, msg, length, 0);
}

// Broadcast a message from a client
void TcpListener::broadcastToClients(int sendingClient, const char* msg, int length)
{
	for (int i{ 0 }; i < m_master.fd_count; ++i)
	{
		SOCKET outSock{ m_master.fd_array[i] };
		if (outSock != m_socket && outSock != sendingClient)
		{
			sendToClient(outSock, msg, length);
		}
	}
}
