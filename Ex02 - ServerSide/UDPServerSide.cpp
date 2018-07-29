#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
// Don't forget to include "Ws2_32.lib" in the library list.
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sstream>

#define TIME_PORT	27015
#define BUFFER_SIZE	255

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

bool recieveFromClient(SOCKET io_Socket,char * i_RecvBuff,sockaddr &io_ClientAddress, int &io_ClientAddressLen)
{
	int bytesRecv = 0;
	bytesRecv = recvfrom(io_Socket, i_RecvBuff, 255, 0, &io_ClientAddress, &io_ClientAddressLen);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
		closesocket(io_Socket);
		WSACleanup();
		return false;
	}

	i_RecvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << i_RecvBuff << "\" message.\n";

	return true;
}

bool sendAnswerToClient(SOCKET io_Socket, char* i_SendBuff, int i_SendBuffLen, sockaddr i_ClientAddress, int i_ClientAddressLen)
{
	int bytesSent = 0;

	bytesSent = sendto(io_Socket, i_SendBuff, (int)strlen(i_SendBuff), 0, (const sockaddr *)&i_ClientAddress, i_ClientAddressLen);
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(io_Socket);
		WSACleanup();
		return false;
	}

	cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(i_SendBuff) << " bytes of \"" << i_SendBuff << "\" message.\n";

	return true;
}

void GetTime(char * o_SendBuff)	// 1
{
	// Get the current time.
	time_t timer;
	time(&timer);
	// Parse the current time to printable string.
	strcpy(o_SendBuff, ctime(&timer));
	o_SendBuff[strlen(o_SendBuff) - 1] = '\0'; //to remove the new-line from the created string
}

void GetTimeWithoutDate(char * o_SendBuff)	// 2
{
	time_t timer;

	time(&timer);
	strftime(o_SendBuff, 255, "%H:%M:%S", localtime(&timer));
}

void GetTimeSinceEpoch(char * o_SendBuff)	// 3
{
	time_t timer = time(nullptr);
	int secs = (int)timer;
	sprintf(o_SendBuff, "%d", secs);
}

void GetClientToServerDelayEstimation(char * o_SendBuff)	// 4
{

}

void MeasureRTT(char * O_SendBuff)		// 5
{

}

void GetTimeWithoutDateOrSeconds(char * o_SendBuff)		// 6
{
	time_t timer;

	time(&timer);
	strftime(o_SendBuff, 255, "%H:%M", localtime(&timer));
}

void GetYear(char * o_SendBuff)		// 7
{
	time_t timer;

	time(&timer);
	strftime(o_SendBuff, 255, "%Y", localtime(&timer));
}

void GetMonthAndDay(char * o_SendBuff)		// 8
{
	time_t timer;

	time(&timer);
	strftime(o_SendBuff, 255, "%d/%m", localtime(&timer));
}

void GetSecondsSinceBeginingOfMonth(char * o_SendBuff)		// 9
{
	int secsSinceBeginingOfMonth;
	time_t timer1 = time(&timer1), timer2;
	tm* currTime = localtime(&timer1);
	tm* beginingOfMonth = localtime(&timer1);

	beginingOfMonth->tm_mday = 1;
	beginingOfMonth->tm_hour = 0;
	beginingOfMonth->tm_min = 0;
	beginingOfMonth->tm_sec = 0;
	timer2 = mktime(beginingOfMonth);
	secsSinceBeginingOfMonth = (int)timer1 - (int)timer2;
	sprintf(o_SendBuff, "%d", secsSinceBeginingOfMonth);
}

void GetDayOfYear(char *o_SendBuff)		// 10
{
	time_t timer;

	time(&timer);
	strftime(o_SendBuff, 255, "%j", localtime(&timer));
}

void GetDaylightSavings(char *o_SendBuff)		// 11
{
	time_t timer = time(&timer);
	tm* localTime = localtime(&timer);

	sprintf(o_SendBuff, "%d", localTime->tm_isdst);
}

void startService()
{
	SOCKET m_socket;
	sockaddr_in serverService;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];
	int userChoice;
	bool sendMsg = true;

	if (!prepareForService(m_socket, serverService))
	{
		return;
	}

	// Waits for incoming requests from clients.

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
		if (!recieveFromClient(m_socket, recvBuff, client_addr, client_addr_len))
		{
			return;
		}
		// Answer client's request by the current time.

		userChoice = stoi(recvBuff);
		switch (userChoice)
		{
		case 1:
			GetTime(sendBuff);                    // V
			sendMsg = true;
			break;
		case 2:
			GetTimeWithoutDate(sendBuff);		 // V
			sendMsg = true;
			break;
		case 3:
			GetTimeSinceEpoch(sendBuff);         // V
			sendMsg = true;
			break;
		case 4:
			GetClientToServerDelayEstimation(sendBuff);
			sendMsg = true;
			break;
		case 5:
			MeasureRTT(sendBuff);
			sendMsg = true;
			break;
		case 6:
			GetTimeWithoutDateOrSeconds(sendBuff);   // V
			sendMsg = true;
			break;
		case 7:
			GetYear(sendBuff);					 // V
			sendMsg = true;
			break;
		case 8:
			GetMonthAndDay(sendBuff);			// V
			sendMsg = true;
			break;
		case 9:
			GetSecondsSinceBeginingOfMonth(sendBuff);	//V
			sendMsg = true;
			break;
		case 10:
			GetDayOfYear(sendBuff);				// V
			sendMsg = true;
		case 11:
			GetDaylightSavings(sendBuff);		// V
			sendMsg = true;
			break;
		default:
			sendMsg = false;
			break;
		}
		
		if (sendMsg)
		{
			if (!sendAnswerToClient(m_socket, sendBuff, (int)strlen(sendBuff), client_addr, client_addr_len))
			{
				return;
			}
		}
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