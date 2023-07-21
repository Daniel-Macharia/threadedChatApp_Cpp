//I want this app to have an infinite loop for sending 
//and another infinte loop for recieving
//running on separate threads from the main

#include<iostream>
#include<ws2tcpip.h>
#include<string>
#include<bitset>
#include<chrono>
#include<thread>
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::bitset;
using std::thread;

const int size = 1000;
int flags = 0;
char sBuf[size] = "";
char rBuf[size] = "";

char messages[10000] = "================== a commandline chat aplication ==================\n\n";

void sendData(SOCKET s )
{
	while( strcmp(sBuf, "shutdown") != 0)
	{
		cout<<"Enter Message: ";
		cin.getline(sBuf, size); 
		if( send(s, sBuf, size, flags) > 0)
		{
			strcat(messages, "Sent: ");
			strcat(messages, sBuf);
			strcat(messages, "\n");
			system("cls");
			cout<<messages<<endl;
		}
		else
		{
			cout<<"Error sending message: "<<WSAGetLastError()<<endl;
		}
	}
	
	strcpy(rBuf, sBuf);
	cout<<"------------exiting sender thread---------------"<<endl;
}

void recieveData(SOCKET s )
{
	int byteCount = 0;
	while( strcmp( rBuf, "shutdown") != 0)
	{
		byteCount = recv(s, rBuf, size, flags);
		if(byteCount > 0)
		{
			strcat(messages, "Recieved: ");
			strcat(messages, rBuf);
			strcat(messages, "\n");
			system("cls");
			cout<<messages<<endl;
			cout<<"Enter Message: ";
		}
		else
		{
			cout<<"No Message Recieved.."<<endl;
		}
	}
	strcpy(sBuf, rBuf);
	cout<<"------------exiting reciever thread---------------"<<endl;
}

void chat(SOCKET s)
{
	//create two separate threads for sending and recieving data 
   	thread sender(sendData, s);
   	thread receiver( recieveData, s);
   	sender.join();
   	receiver.join();
   
}

int main()
{
	//load the winsock dll
	WSADATA wsadata;
	int wsaerr;
	WORD wVersionRequested = MAKEWORD(2, 2);
	
   wsaerr = WSAStartup(wVersionRequested, &wsadata);
   
   if( wsaerr == 0)
   {
   	cout<<"winsock dll loaded successfully"<<endl;
   }
   else
   {
   	cout<<"failed to load the winsock dll"<<WSAGetLastError()<<endl;
   	return -1;
   }
   
   //create a socket for the server
   SOCKET server = INVALID_SOCKET;
   
   server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   
   if( server == INVALID_SOCKET )
   {
   	cout<<"Failed to create server socket"<<WSAGetLastError()<<endl;
   	WSACleanup();
   	return -1;
   }
   else
   {
   	cout<<"Successfully created server socket"<<endl;
   }
   
   //bind the socket to some ip address and a port number
   sockaddr_in address;
   address.sin_family = AF_INET;
   
   //represent the loopback ip as bits
   //              1    .    0   .   0    .   127
   string s = "00000001000000000000000001111111";
   bitset<32> b(s);
   address.sin_addr.s_addr = b.to_ulong();
   int port = 55550;
   address.sin_port = htons(port);
   
   if( bind(server, (SOCKADDR*) &address, sizeof(address)) == 0)
   {
   	cout<<"successfully bound server to ip and port numer"<<endl;
   }
   else
   {
   	cout<<"failed to bind server to ip and port "<<WSAGetLastError()<<endl;
   	closesocket(server);
   	WSACleanup();
   	return -1;
   }
   
   //listen on that ip and port for client(s)
   
   if( listen(server, 1) == 0)
   {
 	cout<<"Listening..."<<endl;
   }
   else
   {
   	cout<<"Failed to listen "<<WSAGetLastError()<<endl;
   	closesocket(server);
   	WSACleanup();
   	return -1;
   }
   
   //accept connections from the client
   SOCKET acceptCon = INVALID_SOCKET;
   
   acceptCon = accept(server, NULL, NULL);
   
   if( acceptCon == INVALID_SOCKET )
   {
   	cout<<"failed to accept connection from client"<<WSAGetLastError()<<endl;
   	closesocket(server);
   	WSACleanup();
   	return -1;
   }
   else
   {
   	cout<<"Succesfully connected to client\n"<<"Ready to chat"<<endl;
   	//on successful connection begin chatting
   	chat(acceptCon);
   
   }
   
   
   	cout<<"Bye"<<endl;
    std::this_thread::sleep_for( std::chrono::milliseconds(5000));
    
	return 0;
}
