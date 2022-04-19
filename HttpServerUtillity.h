#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>

#define HTTP_PORT 8080
#define MAX_SOCKETS 60
#define EMPTY  0
#define LISTEN 1
#define RECEIVE 2
#define IDLE 3
#define SEND 4
#define POST 6
#define GET 5
#define DELETE 7
#define PUT 8
#define HEAD 9
#define TRACE 10
#define OPTIONS 11
#define ERROR 12

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	int sendSubType;	// Sending sub-type
	char buffer[10000];
	int len;
	time_t lastMessageTime;
};


bool addSocket(SOCKET id, int what, SocketState* sockets, int* socketsCount);
void removeSocket(int index, SocketState* sockets, int* socketsCount);
void acceptConnection(int index, SocketState* sockets, int* socketsCount);
void receiveMessage(int index, SocketState* sockets, int* socketsCount);
void sendMessage(int index, SocketState* sockets, int* socketsCount);
void checkTimeout(SocketState* sockets, int* socketsCount);
void RunServer();

