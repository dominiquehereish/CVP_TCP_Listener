#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void setListener();
void waitConnection(SOCKET, sockaddr_in);

int main()
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return 0;
	}

	// Create a socket to listen for a connection
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return 0;
	}

	// Goes to the function that will create an inifinite loop to listen
	setListener();

	// Cleanup winsock
	WSACleanup();

	system("pause");

}

//setup the socket for listening
void setListener() {
	// Create a socket to listen for a connection
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in in;
	in.sin_family = AF_INET;
	in.sin_port = htons(5400);
	// Setting IP to INADDR_ANY, could configure your IP manually
	in.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton 
	//call fct to wait for a connection
	waitConnection(listening, in);
}

//wait for a connection and receive the message
void waitConnection(SOCKET listening, sockaddr_in in) {

	bind(listening, (sockaddr*)&in, sizeof(in));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);
	// Tell we are listening
	cout << "Server listening on port 5400 ..." << endl;
	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Port the client is connected on

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	//Show client is connected
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}

	// Close listening socket
	closesocket(listening);

	int i = 1;
	char buf[4096];

	//Infinite loop to receive messages for client
	do
	{
		ZeroMemory(buf, 4096);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << endl;
			break;
		}

		if (bytesReceived == 0)
		{
			cout << "Client disconnected " << endl;
			break;
		}

		cout << string(buf, 0, bytesReceived) << endl;


	} while (i > 0);

	// Close the socket
	closesocket(clientSocket);
	// Loop back to setup the listener
	setListener();
}
