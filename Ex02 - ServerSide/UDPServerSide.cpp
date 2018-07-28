#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
// Don't forget to include "Ws2_32.lib" in the library list.
#include <winsock2.h>
#include <string.h>
#include <time.h>

#define TIME_PORT	27015

bool prepareForService(SOCKET &io_Socket, sockaddr_in &io_SocketService)
{
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
		cout << "Time Server: Error at WSAStartup()\n";
		return false;
	}

	// Server side:
	// Create and bind a socket to an internet address.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	io_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == io_Socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	// For a server to communicate on a network, it must first bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Address family (must be AF_INET - Internet address family).
	io_SocketService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	io_SocketService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
												// IP Port. The htons (host to network - short) function converts an
												// unsigned short from host to TCP/IP network byte order (which is big-endian).
	io_SocketService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(io_Socket, (SOCKADDR *)&io_SocketService, sizeof(io_SocketService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(io_Socket);
		WSACleanup();
		return false;
	}

	return true;
}

void recieveFromClient(SOCKET io_Socket,char * i_RecvBuff,sockaddr &io_ClientAddress, int &io_ClientAddressLen)
{
	int bytesRecv = 0;
	bytesRecv = recvfrom(io_Socket, i_RecvBuff, 255, 0, &io_ClientAddress, &io_ClientAddressLen);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
		closesocket(io_Socket);
		WSACleanup();
		return;
	}

	i_RecvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << i_RecvBuff << "\" message.\n";
}

void startService()
{
	SOCKET m_socket;
	sockaddr_in serverService;
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];

	if (!prepareForService(m_socket, serverService))
	{
		return;
	}

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);


	// Get client's requests and answer them.
	// The recvfrom function receives a datagram and stores the source address.
	// The buffer for data to be received and its available size are 
	// returned by recvfrom. The fourth argument is an idicator 
	// specifying the way in which the call is made (0 for default).
	// The two last arguments are optional and will hold the details of the client for further communication. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		
		recieveFromClient(m_socket, recvBuff, client_addr, client_addr_len);
		// Answer client's request by the current time.

		// Get the current time.
		time_t timer;
		time(&timer);
		// Parse the current time to printable string.
		strcpy(sendBuff, ctime(&timer));
		sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string

											   // Sends the answer to the client, using the client address gathered
											   // by recvfrom. 
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr *)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}

void main()
{
	startService();
}