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

int writen(int, char*, int);
int readline(int , char*, int );

int main (int argc, char** argv) {

	const int 			BACKLOG = 10;		// parameter for listen() call
	size_t 				MAXLINE = 255;
	int 				MAXNUMCLIENTS = 32;
	int 				MAX_CLIENTS;

	int 				servport, listenfd, clientfd, sockfd, maxfd, maxk, numready, k;
	int 				numSend = 0;
	int 				numRecv = 0;
	int 				client[MAXNUMCLIENTS];
	ssize_t				n;

	fd_set				rset, allset;

	struct sockaddr_in 	server_addr, client_addr;
	socklen_t			client_len;
	pid_t				child_pid;

	uint8_t				buffer[MAXLINE];
	uint8_t*			end;

	SBPCMessage			message;
	SBCPAttribute*		attrList[16] = {0};
	int 				length;



	// Get port number from command line
	if (argc != 3){
		std::cout << "Usage: server <port_no> <max_clients>\n" << std::endl;
		return 0;
	}
	servport = atoi(argv[1]);
	std::cout << "Port_Num: " << servport << std::endl;
	
	// Preliminary stuff
	bzero(&server_addr,sizeof(server_addr));
	bzero(&client_addr,sizeof(client_addr));
	server_addr.sin_family 		= AF_INET;				// use IPv4 protocol
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// use any interface
	server_addr.sin_port 		= htons(servport);		// use portno provided by user

	// Create socket
	if ( (listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	// std::cout << "Socket created successfully." << std::endl;
	// Bind socket to port/IP address
	if ( (bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ) != 0 ){
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	// std::cout << "Bind successful." << std::endl;

	// processing loop
	// accepts new clients, uses select to read/write to available client fds.
	for (;;){

		// fork process to handle new client


		// server child: recvfrom()

		// processing

		// server child: sendto()
	}

	return 0;
}