// Echo Server
// Written by: Derek Heidtke and Ryan Hill

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int writen(int, char*, int);
int readline(int , char*, int );

int main (int argc, char** argv) {

	int 				servport, sockfd, clientfd;
	int 				numSend = 0;
	int 				numRecv = 0;

	struct sockaddr_in 	server_addr, client_addr;
	socklen_t			client_len;
	pid_t				child_pid;

	size_t 				MAXLINE = 255;
	char 				buffer[MAXLINE];

	const int BACKLOG = 10;		// parameter for listen() call

	// Get port number from command line
	if (argc != 2){
		std::cout << "Usage: echos [port_no]\n" << std::endl;
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
	if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	// std::cout << "Socket created successfully." << std::endl;

	// Bind socket to port/IP address
	if ( (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ) != 0 ){
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	// std::cout << "Bind successful." << std::endl;

	// Listen for clients on listening port
	if ( (listen(sockfd,BACKLOG)) < 0 ) {
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	// std::cout << "Listen successful." << std::endl;

	// accept loop
	for (;;){

		// accept connection from incoming client
		std::cout << "Waiting for client ..." << std::flush;
		client_len = sizeof(client_addr);
		if ((clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len)) < 0){
			std::cout << strerror(errno) << std::endl;
			return 0;
		}
		std::cout << "Client connected!" << std::endl;

		// Fork new process to handle client's request (i.e. echo)
		if ( (child_pid = fork()) < 0){
			std::cout << strerror(errno) << std::endl;
			return 0;
		}
		if (child_pid == 0) { // child process:
			// close listening socket descriptor
			if ( (close(sockfd)) != 0 ) {
				std::cout << strerror(errno) << std::endl;
			}
			// process client's request ============================================

			// read data from client
			// clear buffer before use
			bzero( buffer, MAXLINE);

			// read socket descriptor
			if ( readline(clientfd, buffer, MAXLINE) < 0){
				perror("server readline()");
				break;
			}

			printf("From client: %s", buffer);
			std::cout << std::endl;


			writen(clientfd, buffer, strlen(buffer));

			// exit ===============================================================
			exit(0);
		}
		// parent process: (close previous client socket and listen for next client)
		if ( (close(clientfd)) != 0 ) {
			std::cout << strerror(errno) << std::endl;
		}
	}

	return 0;
}

// robust loop to send string across connection-based socket
int writen(int sockfd, char* buffer, int length){

	int		err = 0;
	int		count = 0;	// keep track of how many chars the client sent to the server
	
	// write to socket descriptor
	while ( count != length ) {
		err = send(sockfd,buffer,length+1,0);

		if (err == -1 && errno == EINTR) {
			// try again if interrupted
			continue;
		} else if (err == -1) {
			// break if more serious error
			perror("writen()");
			return -1;
		}

		// keep track of sent characters
		count += err;

		// should never happen
		if (count > length) {
			// printf("\nWRITEN() ERROR!");
			return -1;
		}
	}

	return count;

}

// robust readline from socket connection
int readline(int sockfd, char* buffer, int buflength) {

	int		err = 0;
	int		count = 0;	// keep track of how many chars the client sent to the server

	while ( count < buflength ) {

		err = recv(sockfd, buffer, buflength, 0);

		// if end of file, i.e. if no newline found in string
		if ( strstr(buffer, "\n") != NULL ) {
			break;
		}

		if (err == 0) { // also, end of file
			break;
		}

		if (err == -1 && errno == EINTR) {
			// try again if interrupted
			continue;
		} else if (err == -1) {
			perror("readline()");
			return -1;
		}

		count += err;


		// should not happen
		if (count > buflength) {
			printf("\nREADLINE() ERROR!");
			break;
		}
	}

	return count;
}