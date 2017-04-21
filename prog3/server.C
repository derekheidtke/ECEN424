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

	int 	servport, sockfd;
	struct 	sockaddr_in servaddr, cliaddr;

	char* 	servAddrIP;

	pid_t 	child_pid;


	// Get port number from command line
	if (argc != 3){
		std::cout << "Usage: server <IP_addr> <port_no>\n" << std::endl;
		return 0;
	}
	servAddrIP = argv[1];
	servport = atoi(argv[2]);
	// std::cout << servAddrIP << " " << servport << std::endl;
	

	// Create socket
	if ( (sockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0 ){
		std::cerr << "\nERROR: " << std::flush;
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	std::cout << "Parent socket created successfully." << std::endl;

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family			= PF_INET;				// use UDP datagram Protocol
	servaddr.sin_addr.s_addr	= htonl(INADDR_ANY);	// use any interface
	servaddr.sin_port			= htons(servport);		// use portno provided by user

	// Bind socket to port/IP address
	if ( (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) ) != 0 ){
		std::cerr << "\nERROR: " << std::flush;
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	std::cout << "Parent bind successful." << std::endl;

	for (;;){

		int			n;
		socklen_t	len = sizeof(cliaddr);
		char		mesg[MAXLINE];
		TFTPMESSAGE	message;

		// blocking read of next datagram
		n = recvfrom(sockfd, (void*)mesg, (size_t)MAXLINE, 0, (struct sockaddr*) &cliaddr, &len);
		if (n == -1){
			std::cerr << "\nERROR: " << len << " " << std::flush;
			std::cout << strerror(errno) << std::endl;
			return 0;
		}

		tftpDecode(mesg, &message);

		// std::cout << "\nopcode: " << message.opcode << std::flush;
		// std::cout << "\nseqNum: " << message.seqNum << std::flush;
		// std::cout << "\nerror: "  << message.error  << std::flush;
		// std::cout << "\nfilename: " << message.filename << std::flush;

		// ====================================================================================

		// fork process to handle new RRQ request
		child_pid = fork();
		if ( child_pid < 0 ){
			std::cout << strerror(errno) << std::endl;
			return 0;
		}
		if (child_pid == 0) { // child process: handle RRQ request

			// if decoded message is RRQ
			if (message.opcode == TFTP_RRQ) {

				int 	newSockfd;
				struct 	sockaddr_in childaddr;

				// close existing socket descriptor from parent
				if ( (close(sockfd)) != 0 ) {
					std::cout << strerror(errno) << std::endl;
				}

				// bind new ephemeral socket
				if ( (newSockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0 ){
					std::cerr << "\nERROR: " << std::flush;
					std::cout << strerror(errno) << std::endl;
					return 0;
				}
				bzero(&servaddr,sizeof(servaddr));
				servaddr.sin_family			= PF_INET;				// use UDP datagram Protocol
				servaddr.sin_addr.s_addr	= htonl(INADDR_ANY);	// use any interface
				servaddr.sin_port			= htons(0);				// use ephemeral port

				// Bind socket to port/IP address
				if ( (bind(newSockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) ) != 0 ){
					std::cerr << "\nERROR: " << std::flush;
					std::cout << strerror(errno) << std::endl;
					return 0;
				}
				std::cout << "\nChild socket created successfully." << std::flush;

				// send data file to client
				CLIRECORD clientInfo;
				strcpy(clientInfo.filename, message.filename);
				clientInfo.seqNum 			= 0;
				clientInfo.lastSegmentSent 	= 0;
				clientInfo.cliaddr 			= cliaddr;
				clientInfo.cliSockfd 		= newSockfd;

				tftpRespondRRQ(&message, (struct sockaddr*) &cliaddr, &clientInfo);

				while(clientInfo.lastSegmentSent != 1) {
					n = recvfrom(newSockfd, mesg, MAXLINE, 0, (struct sockaddr*) &cliaddr, &len);
					tftpDecode(mesg, &message);

					if (message.opcode == TFTP_ACK) {
						std::cout << "\nWe got an ACK ... :| " << std::flush;

						tftpRespondACK(&message, (struct sockaddr*)&cliaddr, &clientInfo);

						
					} else if (message.opcode == TFTP_ERR) {
						std::cout << "\nWe got an ERR ... :| " << std::flush;
					} else if (message.opcode == TFTP_RRQ) {
						std::cout << "\nWe got a redundant RRQ ... :| " << std::flush;
					} else {
						std::cerr << "\nERROR: Unknown TFTP message type." << std::flush;
						return -1;
					}
				}


				std::cout << "\nChild closing." << std::flush;
				return 0;

			} 
		} // child block




	} // forever loop

	return 0;
}