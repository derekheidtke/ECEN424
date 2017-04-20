// Echo Client
// Written by: Derek Heidtke and Ryan Hill

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string.h>

#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "headers.H"


int writen(int, const char*, int);
int readline(int , char*, int );

void cli_proc(FILE*, int, int);


int main (int argc, char** argv) {

	int 				SERVPORT, sockfd;//, serverfd;
	int numSend = 0;
	int numRecv = 0;

	struct sockaddr_in 	serv_addr;
	char*				servAddrsString;

	std::string 		inputString;

	size_t 				MAXLINE = 255;
	char 				sendBuffer[MAXLINE];
	char 				recvBuffer[MAXLINE];

	// Get IP addr and port number of server from command line
	if (argc != 4){
		std::cout << "Usage: client <username> <IPv4_addr> <port_no>\n" << std::endl;
		return 0;
	}

	servAddrsString = argv[2];
	std::cout << "IP_addr: " << servAddrsString << std::endl;

	SERVPORT = atoi(argv[3]);
	std::cout << "Port_Num: " << SERVPORT << std::endl;

	// Preliminary stuff
	// Create socket
	if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
		perror("socket()");
		return 0;
	}
	// std::cout << "Socket created successfully." << std::endl;

	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;				// use IPv4 protocol
	serv_addr.sin_port 			= htons(SERVPORT);		// use portno provided by user
	if ( inet_pton(AF_INET, servAddrsString, &serv_addr.sin_addr) != 1 ) {
		perror("inet_pton()");
		return 0;
	}

	// Connect to server
	if ( (connect(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) != 0 ) {
		perror("connect()");
		return 0;
	}
	std::cout << "Connected to server!" << std::endl;

	// do the stuff with the server
	cli_proc(stdin, sockfd, MAXLINE);


	exit(0);
}

void cli_proc(FILE *fp, int sockfd, int MAXLINE) {

	int 	maxfdp1;
	int 	stdinEOF = 0;
	fd_set	rset;
	uint8_t 	buffer[MAXLINE] = {0};
	uint8_t*	end;

	int 	num;

	SBCPMessage		message;
	SBCPAttribute*	attrList[16];

	FD_ZERO(&rset);

	for(;;) {
		// if server has not closed connection yet
		if (stdinEOF == 0) {
			FD_SET(fileno(fp),&rset);
		}
		// then input from server
		FD_SET(sockfd, &rset);

		maxfdp1 = std::max(fileno(fp), sockfd) + 1;

		// get input from keyboard or server
		if (select(maxfdp1, &rset, NULL, NULL, NULL) < 0) {
			std::cout << strerror(errno) << std::endl;
		}

		// if server sends something
		if (FD_ISSET(sockfd,&rset)) {
			// if nothing read from server
			if ( (num=read(sockfd,buffer,MAXLINE)) == 0 ) {
				// if server closes connection
				if (stdinEOF == 1) {
					return;
				} else {
					perror("cli_proc: server closed unexpectedly");
					return;
				}
			}
			std::cout << "Server: " << buffer << std::flush;

			attrList[0] = (SBCPAttribute*)malloc(sizeof(SBCPAttribute*));
			deserializePacket(buffer,&message, attrList);
			printMessage(&message);
			printAttribute(attrList[0]);


			// write(fileno(stdout), buffer, num);

			// reset all data structures: buffer, message, attrList
			bzero(buffer,MAXLINE);
			free(attrList[0]);
		}

		// if input from keyboard
		if (FD_ISSET(fileno(fp), &rset)) {
			std::cout << "\n> " << std::flush;
			// if
			if ( (num = read(fileno(fp), buffer, MAXLINE)) == 0 ) {
				stdinEOF = 1;
				if (shutdown(sockfd,SHUT_WR) < 0) {
					perror("shutdown error");
				}
				FD_CLR(fileno(fp), &rset);
				continue;
			}

			// initialize message and attributes to be sent over
			attrList[0] = (SBCPAttribute*)malloc(sizeof(SBCPAttribute*));
			createAttr(attrList[0],ATTR_TYPE_MESS, 4+strlen((char*)buffer));
			attrList[0]->payload = (char*)buffer;
			// strcpy(attrList[0].payload,"abcdefg");

			createMess(&message,3,MESS_TYPE_FWD,4+attrList[0]->length);

			// prepare buffer for sending (serialization)
			serializePacket(buffer, MAXLINE, message,attrList,1);

			std::cout << "Sending: " << buffer << std::flush;
			write(sockfd,buffer,MAXLINE);

			// reset all data structures: buffer, message, attrList
			bzero(buffer,MAXLINE);
			free(attrList[0]);
		}
	}
}


// // robust loop to send string across connection-based socket
// int writen(int fd, const char* buffer, int n){

// 	// size_t 		numLeft;
// 	// ssize_t 	numWritten;
// 	// const char*	ptr;

// 	// ptr = buffer;
// 	// numLeft = n;

// 	int		err = 0;
// 	int		count = 0;	// keep track of how many chars the client sent to the server
	
// 	// write to socket descriptor
// 	while ( count != n ) {
// 		err = send(fd,buffer,n+1,0);

// 		if (err == -1 && errno == EINTR) {
// 			// try again if interrupted
// 			continue;
// 		} else if (err == -1) {
// 			// break if more serious error
// 			perror("writen()");
// 			return -1;
// 		}

// 		// keep track of sent characters
// 		count += err;

// 		// should never happen
// 		if (count > n) {
// 			// printf("\nWRITEN() ERROR!");
// 			return -1;
// 		}
// 	}

// 	return count;

// }

// // robust readline from socket connection
// int readline(int sockfd, char* buffer, int buflength) {

// 	int		err = 0;
// 	int		count = 0;	// keep track of how many chars the client sent to the server

// 	while ( count < buflength ) {

// 		err = recv(sockfd, buffer, buflength, 0);

// 		// if end of file, i.e. if no newline found in string
// 		if ( strstr(buffer, "\n") != NULL ) {
// 			break;
// 		}

// 		if (err == 0) { // also, end of file
// 			break;
// 		}

// 		if (err == -1 && errno == EINTR) {
// 			// try again if interrupted
// 			continue;
// 		} else if (err == -1) {
// 			perror("readline()");
// 			return -1;
// 		}

// 		count += err;


// 		// should not happen
// 		if (count > buflength) {
// 			printf("\nREADLINE() ERROR!");
// 			break;
// 		}
// 	}

// 	return count;
// }