// Echo Client
// Written by: Derek Heidtke and Ryan Hill

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main (int argc, char** argv) {

	int 				servport, sockfd;//, serverfd;
	struct sockaddr_in 	serv_addr, client_addr;
	char*				servAddrsString;
	socklen_t			server_len;
	in_addr_t			serverAddressIPv4;

	// Get IP addr and port number of server from command line
	if (argc != 3){
		std::cout << "Usage: echos [IPv4_addr] [port_no]\n" << std::endl;
		return 0;
	}

	servAddrsString = argv[1];
	std::cout << "IP_addr: " << servAddrsString << std::endl;

	servport = atoi(argv[2]);
	std::cout << "Port_Num: " << servport << std::endl;

	// Preliminary stuff
	bzero(&serv_addr,sizeof(client_addr));
	client_addr.sin_family 		= AF_INET;				// use IPv4 protocol
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// use any interface
	client_addr.sin_port 		= htons(0);				// use ephemeral port

	inet_pton(AF_INET,servAddrsString,&serverAddressIPv4);
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;				// use IPv4 protocol
	serv_addr.sin_addr.s_addr 	=  serverAddressIPv4;	// use addr provided by user
	serv_addr.sin_port 			= htons(servport);		// use portno provided by user

	// Create socket
	if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	std::cout << "Socket created successfully." << std::endl;

	// Bind socket to port/IP address
	if ( (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) ) != 0 ){
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	std::cout << "Bind successful." << std::endl;

	// // Connect to server
	// if ( (connect(sockfd, (struct sockaddr*),)) != 0 ) {
	// 	std::cout << strerror(errno) << std::endl;
	// 	return 0;
	// }

	return 0;
}
