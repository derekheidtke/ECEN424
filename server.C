// Echo Server
// Written by: Derek Heidtke and Ryan Hill

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void writen();
int readline(int , char*, int );

int main (int argc, char** argv) {

	int 				servport, sockfd, clientfd;
	struct sockaddr_in 	server_addr, client_addr;
	socklen_t			client_len;
	pid_t				child_pid;

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
			// process client's request
			// readline(clientfd);

			// exit
			exit(0);
		}
		// parent process: (close previous client socket and listen for next client)
		if ( (close(clientfd)) != 0 ) {
			std::cout << strerror(errno) << std::endl;
		}
	}

	return 0;
}

// exit function either when newline/EOF appears or if maximum string length occurs
int readline(int client_sock, char* buffer, int max_length){

	int		n = 0;				// keep track of how many chars the client sent to the server
	int		errVal = 0;

	std::cin;
	// while ( n < max_length ) {
	// 	// Read some from socket descriptor
	// 	errVal = recv(client_sock, (void*)&buffer, max_length, 0);

	// 	// Check for EOF
	// 	if (errVal == 0) {
	// 		break;
	// 	}

	// 	// Check for newline
	// 	if ( (strstr(buffer,"\n"))!=NULL ){
	// 		// store newline and null terminator
	// 		buffer[n] = "\0";
	// 		return n;
	// 	}


	// 	// Check for EINTR

	// 	// print message to output
	// 	std::cout << "\nRecieved: " << read_buffer << std::flush;

	// 	// send message back
	// 	while ( (send(client_sock,(void*)read_buffer,BUFSIZE, 0) ) != 0) {
	// 		if (errno == EINTR)
	// 	}
		

	// }

	return n;

}