#pragma once

#include <WinSock2.h>

struct TCPReceiver_Exception
{
	enum ExceptionType
	{
		WSAINITERROR,
		SOCKETINITERROR,
		SOCKETBINDERROR,
		DISCONNECT,
		NONBLOCKING,
		UNKNOWN
	};

	ExceptionType e_type;

	TCPReceiver_Exception(ExceptionType e_type):e_type(e_type)
	{
	}
};

class TCPReceiver
{
public:
	TCPReceiver();
	TCPReceiver(int port);
	~TCPReceiver();

	void Listen(int backlog);
	bool Accept();
	void SetNonBlocking();
	void Receive(char *buff, size_t size);
	int Close();
	int CloseSender();
	bool isClosed();
private:
	bool hasClosed;
	SOCKET m_socket;
	SOCKET m_senderSocket;
	SOCKADDR_IN m_servaddr;
};

