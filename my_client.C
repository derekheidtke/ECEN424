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

	int 				SERVPORT, sockfd;//, serverfd;
	struct sockaddr_in 	serv_addr;
	char*				servAddrsString;

	std::string 		inputString;

	// Get IP addr and port number of server from command line
	if (argc != 3){
		std::cout << "Usage: echos [IPv4_addr] [port_no]\n" << std::endl;
		return 0;
	}

	servAddrsString = argv[1];
	std::cout << "IP_addr: " << servAddrsString << std::endl;

	SERVPORT = atoi(argv[2]);
	std::cout << "Port_Num: " << SERVPORT << std::endl;

	// Preliminary stuff
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;				// use IPv4 protocol
	serv_addr.sin_port 			= htons(SERVPORT);		// use portno provided by user
	
	if ( inet_pton(AF_INET, servAddrsString, &serv_addr.sin_addr) != 1 ) {
		std::cout << "Address conversion error.\n" << std::flush;
		std::cout << strerror(errno) << std::endl;
		return 0;
	}

	// Create socket
	if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	std::cout << "Socket created successfully." << std::endl;

	// Connect to server
	if ( (connect(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) != 0 ) {
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	std::cout << "Connected to server!" << std::endl;

	std::cout << "\nEnter string: " << std::flush;
	std::cin >> inputString;



	return 0;
}
