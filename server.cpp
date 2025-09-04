#include <iostream>
#include <string> 
#include <winsock2.h>
#include "encryption.hpp"
#include <cstring>
#include <thread>
#include <chrono>

#define PORT 3552
unsigned long clientIP = inet_addr("192.168.1.1");


class server
{
	public:

	int sockfd;
	SOCKET client;
	struct sockaddr_in clientAddress;

		void openSocket()
		{
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in address;	
			address.sin_port = htons(PORT);
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = INADDR_ANY;
			bind(sockfd, (const sockaddr*)&address, sizeof(address)); 
			listen(sockfd, 10);

		}

		void startConnections()
		{
			int clientAddressSize = sizeof(struct sockaddr_in);
			client = accept(sockfd, (sockaddr*)&clientAddress, &clientAddressSize);
			
			if(clientAddress.sin_addr.s_addr == clientIP){
				closesocket(sockfd);
				std::cout << "Connection made with client.\n";
			}else{
				closesocket(client);
				std::cout << "Incorrect client attempting connection.\n";
			}

			if(client >= 0) {
				std::cout << "Connection was made with " << inet_ntoa(clientAddress.sin_addr) << '\n';
			}else{
				std::cout << "Error " << client << " Occured.\n";
			}

		}

		void receivePackets()
		{
			while(true)
			{
				//there is an error here regarding the string concatenation
				static char incData[512];
				int clientSent = recv(client, incData, sizeof(incData) - 1, 0);
				int dataLength = strlen(incData);
				static std::string recvString;
				//make it so it prints only when the delivery is complete (check with \n)
				if(incData[0] != '\0'){
					recvString += incData;
				}else{
					std::cout << recvString << '\n';	
				}

				if(clientSent == 0)
				{
					std::cout << "\nClient disconnected.";
				}
					
				std::memset(incData, 0, sizeof(incData));
				std::this_thread::sleep_for(std::chrono::milliseconds(40));
			}

		}

		void sendPacket(std::string outData)
		{
			size_t sentBytes = 0;

			while(sentBytes < outData.length() - 1)
			{
				int sendReturn = send(client, outData.c_str(), outData.length() - 1, 0);
				
				if(sendReturn < 0){
					std::cout << "Error occured with sending message.";
					break;
				}else{
					sentBytes += sendReturn;
				}
			}
			sentBytes = 0;
		}
		void sendConnection()
		{
			//check if the ip is bryce and if not than just start the server
			int connectAttempt = connect(sockfd, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
			std::cout << "Trying to connect to Bryce...";
			if(connectAttempt == 0)
			{
				std::cout << "Connection successful.";
			}else{
				std::cout << "Connection failed.";
			}
		}

};



int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed\n";
		return 1;
	}


	server server;
	std::cout << "This is a basic texting program, would you like to start it up? ";
	std::string response;
	std::cin >> response;
	
	if(response == "yes" || response == "Yes"){
		server.openSocket();
		server.startConnections();
		std::cout << "Say \\EXIT to end program.";
		std::cout << "\nBegin Typing Below\n\n";
	}else{
		return 0;
	} 
	std::cout << "Are you Bryce? ";
	std::cin >> response;
	if(response != "Yes" || response != "yes")
	{
		server.sendConnection();
		std::thread hooks(&server::receivePackets, &server);
		hooks.detach();
	}

	while(response != "\\EXIT")
	{
		std::cin >> response;
		server.sendPacket(response);
	}
	

	return 0;
}
