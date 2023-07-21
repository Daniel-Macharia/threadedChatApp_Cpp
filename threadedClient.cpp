//I want the send and recieve functions to 
//run on separate threads

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
char messages[10000] = "================== a commandline chat aplication ==================\n\n";
char rBuf[size] = "";
char sBuf[size] = "";

void sendData(SOCKET s )
{	
    int byteCount;
	while( strcmp(sBuf, "shutdown") != 0)
	{
		cout<<"Enter Message: ";
		cin.getline(sBuf, size);
		byteCount = send(s, sBuf, size, flags);
		if( byteCount > 0)
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
	while( strcmp( rBuf, "shutdown") != 0)
	{
		if( recv(s, rBuf, size, flags) > 0)
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
	//load winsock api
	WSADATA wsadata;
	int wsaerr;
	WORD version = MAKEWORD(2, 2);
	
	wsaerr = WSAStartup( version, &wsadata);
	
	if( wsaerr == 0)
	{
		cout<<"Successfully loaded the winsock api"<<endl;
	}
	else
	{
		cout<<"failed to load the winsock api"<<WSAGetLastError()<<endl;
		return -1;
	}
	
	//create a socket
	SOCKET client = INVALID_SOCKET;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( client == INVALID_SOCKET )
	{
		cout<<"failed to create client socket"<<WSAGetLastError()<<endl;
		WSACleanup();
		return -1;
	}
	else
	{
		cout<<"successfully created client socket"<<endl;
	}
	
	//connect to the server
	
	//specify the ip of the server
	string s = "00000001000000000000000001111111";
	bitset<32> b(s);
	
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = b.to_ulong();
	int port = 55550;
	address.sin_port = htons(port);
	
	if( connect(client, (SOCKADDR*) &address, sizeof(address) ) == 0)
	{
		cout<<"successfully connected to the server"<<endl;
		//on successful connection to the server, start chatting
		chat(client);
		
	}
	else
	{
		cout<<"failed to connect to the server "<<WSAGetLastError()<<endl;
		closesocket(client);
		WSACleanup();
		return -1;
	}
	
	cout<<"Bye"<<endl;
	std::this_thread::sleep_for( std::chrono::milliseconds(5000));
	
	return 0;
}
