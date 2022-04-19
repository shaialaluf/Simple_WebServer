#include "HttpServerUtillity.h"
#include "HttpMessageParser.h"
#include "HttpFunctions.h"


void RunServer()
{
	struct SocketState sockets[MAX_SOCKETS] = { 0 };
	int socketsCount = 0;

	cout << "Server start running" << endl;
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Http Server: Error at WSAStartup()\n";
		return;
	}

	// Server side:
	// Create and bind a socket to an internet address.
	// Listen through the socket for incoming connections.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called listenSocket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							streaming sockets (SOCK_STREAM), 
	//							and the TCP/IP protocol (IPPROTO_TCP).
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The if statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == listenSocket)
	{
		cout << "Http Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a server to communicate on a network, it must bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigned long 
	// (4 bytes) data type).
	// inet_addr (Iternet address) is used to convert a string (char *) 
	// into unsigned long.
	// The IP address is INADDR_ANY to accept connections on all interfaces.
	serverService.sin_addr.s_addr = INADDR_ANY;
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order 
	// (which is big-endian).
	serverService.sin_port = htons(HTTP_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Http Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	// Listen on the Socket for incoming connections.
	// This socket accepts only one connection (no pending connections 
	// from other clients). This sets the backlog parameter.
	if (SOCKET_ERROR == listen(listenSocket, 5))
	{
		cout << "Http Server: Error at listen(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}
	addSocket(listenSocket, LISTEN, sockets, &socketsCount);
	struct timeval timeout;

	// Accept connections and handles them one by one.
	while (true)
	{
		// The select function determines the status of one or more sockets,
		// waiting if necessary, to perform asynchronous I/O. Use fd_sets for
		// sets of handles for reading, writing and exceptions. select gets "timeout" for waiting
		// and still performing other operations (Use NULL for blocking). Finally,
		// select returns the number of descriptors which are ready for use (use FD_ISSET
		// macro to check which descriptor in each set is ready to be used).
		timeout.tv_sec = 120;
		timeout.tv_usec = 0;

		// find the active sockets that the server didn't get any message from them for at least 2 minutes - and close their socket.
		// start from index 1 (for not closing listenSocket).
		
		
		fd_set waitRecv;
		FD_ZERO(&waitRecv);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if ((sockets[i].recv == LISTEN) || (sockets[i].recv == RECEIVE))
				FD_SET(sockets[i].id, &waitRecv);
		}

		fd_set waitSend;
		FD_ZERO(&waitSend);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (sockets[i].send == SEND)
				FD_SET(sockets[i].id, &waitSend);
		}

		//
		// Wait for interesting event.
		// Note: First argument is ignored. The fourth is for exceptions.
		// And as written above the last is a timeout, hence we are blocked if nothing happens.
		//
		/*memcpy(&tv1, &tv, sizeof(tv));
		if (select(STDIN + 1, &readfds, NULL, NULL, &tv1) == -1)*/
		int nfd;
		//memcpy(&TEMP, &timeout, sizeof(timeout));
		nfd = select(0, &waitRecv, &waitSend, NULL, &timeout);
		
		if (nfd == SOCKET_ERROR)
		{
			cout << "Http Server: Error at select(): " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}
		checkTimeout( sockets,&socketsCount);		

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitRecv))
			{
				nfd--;
				switch (sockets[i].recv)
				{
				case LISTEN:
					acceptConnection(i, sockets, &socketsCount);
					break;

				case RECEIVE:
					receiveMessage(i, sockets, &socketsCount);
					break;
				}
			}
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitSend))
			{
				nfd--;
				switch (sockets[i].send)
				{
				case SEND:
					sendMessage(i, sockets, &socketsCount);
					break;
				}
			}
		}
	}

	// Closing connections and Winsock.
	cout << "Http Server: Closing Connection.\n";
	closesocket(listenSocket);
	WSACleanup();
	exit(1);
}
void checkTimeout( SocketState* sockets, int* socketsCount)
{
	for (int i = 1; i < *socketsCount; i++)
	{
		int timeDifference = difftime(time(0), sockets[i].lastMessageTime);
		if (timeDifference >= 120)
		{
			cout << "deleteing socket :" << i<< " time: "<< timeDifference<<endl;
			closesocket(sockets[i].id);
			removeSocket(i, sockets, socketsCount);
		}
	}
}
bool addSocket(SOCKET id, int what, SocketState* sockets, int* socketsCount)
{
	//
// Set the socket to be in non-blocking mode.
//
	unsigned long flag = 1;
	if (ioctlsocket(id, FIONBIO, &flag) != 0)
	{
		cout << "Http Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
	}

	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].recv == EMPTY)
		{
			sockets[i].id = id;
			sockets[i].recv = what;
			sockets[i].send = IDLE;
			sockets[i].len = 0;
			sockets[i].lastMessageTime = time(0);
			(*socketsCount)++;
			return (true);
		}
	}
	return (false);
}

void removeSocket(int index, SocketState* sockets, int* socketsCount)
{
	sockets[index].recv = EMPTY;
	sockets[index].send = EMPTY;
	(*socketsCount)--;
}

void acceptConnection(int index, SocketState* sockets, int* socketsCount)
{
	SOCKET id = sockets[index].id;
	struct sockaddr_in from;		// Address of sending partner
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket)
	{
		cout << "Http Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	cout << "Http Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;


	if (addSocket(msgSocket, RECEIVE, sockets, socketsCount) == false)
	{
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
	return;
}

void receiveMessage(int index, SocketState* sockets, int* socketsCount)
{
	SOCKET msgSocket = sockets[index].id;
	int len = sockets[index].len;
	int bytesRecv = recv(msgSocket, &sockets[index].buffer[len], sizeof(sockets[index].buffer) - len, 0);

	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Http Server: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);
		removeSocket(index, sockets, socketsCount);
		return;
	}
	if (bytesRecv == 0)
	{
		closesocket(msgSocket);
		removeSocket(index, sockets, socketsCount);
		return;
	}
	else
	{
		sockets[index].buffer[len + bytesRecv] = '\0'; //add the null-terminating to make it a string
		//cout << "Http Server: Recieved: " << bytesRecv << " bytes of \"" << &sockets[index].buffer[len] << "\" message.\n";
		sockets[index].len += bytesRecv;
		sockets[index].lastMessageTime = time(0);

		if (sockets[index].len > 0)
		{
			string socketBuffer = string(sockets[index].buffer);
			int requestType = GetRequestType(socketBuffer);
			sockets[index].send = SEND;
		}
	}
}


void sendMessage(int index, SocketState* sockets, int* socketsCount)
{
	int bytesSent = 0;
	char sendBuff[10000];

	SOCKET msgSocket = sockets[index].id;
	string currentRequest = HandleCurrentRequest(index, sockets);
	string httpResponse;
	int requestType = GetRequestType(currentRequest);

	switch (requestType) {
	case GET:
		httpResponse = doGet(currentRequest);
		break;

	case POST:
		httpResponse = doPost(currentRequest);
		break;

	case HEAD:
		httpResponse = doHead(currentRequest);
		break;

	case PUT:
		httpResponse = doPut(currentRequest);
		break;

	case TRACE:
		httpResponse = doTrace(currentRequest);
		break;

	case DELETE:
		httpResponse = doDelete(currentRequest);
		break;

	case OPTIONS:
		httpResponse = doOptions(currentRequest);
		break;

	case ERROR: // unimplemented or illegal request type - return an html page with error message.
		httpResponse = handleIllegalRequest(currentRequest);
		break;
	}
	strcpy(sendBuff, httpResponse.c_str());

	bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff), 0);
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "HTTP Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}

	//cout << "HTTP Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";

	if (sockets[index].len == 0)
	{
		sockets[index].send = IDLE;
	}
}


