// Echo Server
// Written by: Derek Heidtke

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>

#include "headers.H"

int main (int argc, char** argv) {

	const int MAXLINE = 512;

	int 	servport, sockfd;
	struct sockaddr_in servaddr, cliaddr;

	char* servAddrIP;


	// Get port number from command line
	if (argc != 3){
		std::cout << "Usage: server <IP_addr> <port_no>\n" << std::endl;
		return 0;
	}
	servAddrIP = argv[1];
	servport = atoi(argv[2]);
	// std::cout << servAddrIP << " " << servport << std::endl;
	

	// Create socket
	if ( (sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0 ){
		std::cerr << "\nERROR: " << std::flush;
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	// std::cout << "Socket created successfully." << std::endl;

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family			= AF_INET;				// use UDP datagram Protocol
	servaddr.sin_addr.s_addr	= htonl(INADDR_ANY);	// use any interface
	servaddr.sin_port			= htons(servport);		// use portno provided by user

	// Bind socket to port/IP address
	if ( (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) ) != 0 ){
		std::cerr << "\nERROR: " << std::flush;
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	// std::cout << "Bind successful." << std::endl;

	for (;;){

		int			n;
		socklen_t	len;
		char		mesg[MAXLINE];
		TFTPMESSAGE	message;

		n = recvfrom(sockfd, mesg, MAXLINE, 0, (struct sockaddr*) &cliaddr, &len);
		if (n == -1){
			std::cerr << "\nERROR: " << std::flush;
			std::cout << strerror(errno) << std::endl;
		}

		tftpDecode(mesg, &message);

		std::cout << "\nopcode: " << message.opcode << std::flush;
		std::cout << "\nseqNum: " << message.seqNum << std::flush;
		std::cout << "\nerror: "  << message.error  << std::flush;
		std::cout << "\nfilename: " << message.filename << std::flush;

		std::cout << "\n" << n << std::flush;
		std::cout << "\n" << mesg << std::flush;

		// fork process to handle new client


		// server child: recvfrom()

		// processing

		// server child: sendto()


	} // forever loop

	return 0;
}