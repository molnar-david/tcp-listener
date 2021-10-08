#pragma once

#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

class TcpListener
{
private:
	const char* m_ipAddress;	// IP address the server will run on
	int				m_port;			// Port # for the web service
	int				m_socket;		// Internal file descriptor for the listening 
	bool			m_running;		// Bool that controls and tells us whether the server is running or not
	fd_set			m_master;		// Master file descriptor set

protected:
	// Handler for client connections
	virtual void onClientConnected(int clientSocket);

	// Handler for client disconnections
	virtual void onClientDisconnected(int clientSocket);

	// Handler for when a message is received from the client
	virtual void onMessageReceived(int clientSocket, const char* msg, int length);

	// Send a message to a client
	void sendToClient(int clientSocket, const char* msg, int length);

	// Broadcast a message from a client
	void broadcastToClients(int sendingClient, const char* msg, int length);

public:
	TcpListener(const char* ipAddress, int port);

	// Initialize the listener
	int init();

	// Run the listener
	int run();

	// Shut the listener down
	void shutdown();

	bool getRunning() const { return m_running; }
	void setRunning(bool b) { m_running = b; }
};
