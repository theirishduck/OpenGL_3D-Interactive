#include <cstdio>
#include <iostream>
#include <WinSock2.h>

#include "TCPReceiver.h"

TCPReceiver::TCPReceiver(): hasClosed(false)
{

}

TCPReceiver::TCPReceiver(int port): hasClosed(false)
{
	WSADATA WsaDat;
	if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0)
	{
		fprintf(stderr, "TCPReceiver(): wsa startup failed\n");
		WSACleanup();
		throw TCPReceiver_Exception::WSAINITERROR;
	}

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		fprintf(stderr, "TCPReceiver(): socket create failed\n");
		WSACleanup();
		throw TCPReceiver_Exception::SOCKETINITERROR;
	}

	m_servaddr.sin_family = AF_INET;
	m_servaddr.sin_addr.s_addr = INADDR_ANY;
	m_servaddr.sin_port = htons(port);

	if (bind(m_socket, (SOCKADDR*)(&m_servaddr), sizeof(m_servaddr)) == SOCKET_ERROR)
	{
		fprintf(stderr, "TCPReceiver(): socket binding failed\n");
		WSACleanup();
		throw TCPReceiver_Exception::SOCKETBINDERROR;
	}
}


TCPReceiver::~TCPReceiver()
{
}

void TCPReceiver::Listen(int backlog)
{
	listen(m_socket, backlog);
}

bool TCPReceiver::Accept()
{
	m_senderSocket = accept(m_socket, NULL, NULL);

	return !(m_senderSocket == SOCKET_ERROR);
}

void TCPReceiver::SetNonBlocking()
{
	u_long iMode = 1;
	ioctlsocket(m_senderSocket, FIONBIO, &iMode);
}

void TCPReceiver::Receive(char *buff, size_t size)
{
	int bytesRecv = recv(m_senderSocket, buff, size, 0);
	int wsaError;

	if (bytesRecv > 0) 
		return;

	if (bytesRecv == SOCKET_ERROR && (wsaError = WSAGetLastError()) == WSAEWOULDBLOCK)
	{
		throw TCPReceiver_Exception(TCPReceiver_Exception::NONBLOCKING);
	}
	else
	{
		throw TCPReceiver_Exception(TCPReceiver_Exception::DISCONNECT);
	}

}

int TCPReceiver::Close()
{
	hasClosed = true;
	return closesocket(m_socket);
}

int TCPReceiver::CloseSender()
{
	return closesocket(m_senderSocket);
}

bool TCPReceiver::isClosed()
{
	return hasClosed;
}