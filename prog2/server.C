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

	char 				buffer[MAXLINE];

	SBPCMessage			message;
	SBCPAttribute*		attrList;
	int 				length;



	// Get port number from command line
	if (argc != 3){
		std::cout << "Usage: server <port_no> <max_clients>\n" << std::endl;
		return 0;
	}
	servport = atoi(argv[1]);
	std::cout << "Port_Num: " << servport << std::endl;
	MAX_CLIENTS = atoi(argv[2]);
	std::cout << "Max_Clients: " << MAX_CLIENTS << std::endl;

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
	// Listen for clients on listening port
	if ( (listen(listenfd,BACKLOG)) < 0 ) {
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	// std::cout << "Listen successful." << std::endl;

	// initialize maxfd to listenfd file descriptor
	maxfd = listenfd;
	maxk = -1;

	for ( k = 0 ; k < MAXNUMCLIENTS ; k++) {
		client[k] = -1;
	}
	// initialize and turn on bit "listenfd"
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	// processing loop
	// accepts new clients, uses select to read/write to available client fds.
	for (;;){
		rset = allset;

		std::cout << "Selecting ...\n" << std::flush;
		// check which clients are ready to be read from
		if ((numready = select(maxfd+1, &rset, NULL, NULL, NULL) ) < 0) {
			std::cout << strerror(errno) << std::endl;
		}

		// if there is a new connection
		if (FD_ISSET(listenfd, &rset)) {

			// accept client and get new file descriptor for it. them?
			client_len = sizeof(client_addr);
			if ((clientfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len)) < 0){
				std::cout << strerror(errno) << std::endl;
				return 0;
			}
		
			// insert new client file descriptor into list of available descr. (client[])
			for ( k = 0; k < MAXNUMCLIENTS ; k++) {
				if (client[k] < 0) {
					client[k] = clientfd;
					break;
				}
			}

			if (k == MAXNUMCLIENTS) {
				std::cout << "too many clients" << std::endl;
				return 0;
			}

			// also add new client to FD set
			FD_SET(clientfd, &allset);
			if (clientfd > maxfd) {
				maxfd = clientfd;
			} if ( maxk < k ) {
				maxk = k;
			}

			// no available descriptors
			if ( --numready <= 0 ) {
				continue;
			}
		}

		// check all clients for data
		for ( k = 0; k <= maxk; k++ ) {
			if ( (sockfd = client[k]) < 0 ){
				continue;
			} if ( FD_ISSET(sockfd, &rset) ) {

				// if client closes connection
				if ( (n = read(sockfd,buffer, MAXLINE) ) == 0 ) {
					// close sockfd

					FD_CLR(sockfd, &allset);
					client[k] = -1;
				} else {
					std::cout << "Client: " << buffer << std::flush;

					// get the new data from client
					networkToStruct(buffer, &message, attrList);

					message.vrsn = 3;
					message.type = MESS_TYPE_FWD;
					message.length = 0;

					std::cerr << "XXXXXXXXXXXXXXXXXX\n"<<std::flush;
					char temp[8] = "abcdefg";
					attrList[0].type = ATTR_TYPE_MESS;
					attrList[0].payload = temp;
					attrList[0].length = 8;


					structToNetwork(buffer,message,attrList,1);


					std::cout << "Writing: " << buffer << std::flush;
					write(sockfd, buffer, MAXLINE);
				}

				// if no more available descriptors
				if ( --numready <= 0 ) {
					break;
				}
			}
		}
	}

	return 0;
}

// robust loop to send string across connection-based socket
int writen(int listenfd, char* buffer, int length){

	int		err = 0;
	int		count = 0;	// keep track of how many chars the client sent to the server
	
	// write to socket descriptor
	while ( count != length ) {
		err = send(listenfd,buffer,length+1,0);

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
int readline(int listenfd, char* buffer, int buflength) {

	int		err = 0;
	int		count = 0;	// keep track of how many chars the client sent to the server

	while ( count < buflength ) {

		err = recv(listenfd, buffer, buflength, 0);

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