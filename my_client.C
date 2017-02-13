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

int writen(int, char*, int);
int readline(int , char*, int );


int main (int argc, char** argv) {

	int 				SERVPORT, sockfd;//, serverfd;
	int numSend = 0;
	int numRecv = 0;

	struct sockaddr_in 	serv_addr;
	char*				servAddrsString;

	std::string 		inputString;

	size_t 				MAXLINE = 1024;
	char 				sendBuffer[MAXLINE];
	char 				recvBuffer[MAXLINE];

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
		perror("inet_pton()");
		return 0;
	}

	// Create socket
	if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
		perror("socket()");
		return 0;
	}
	// std::cout << "Socket created successfully." << std::endl;

	// Connect to server
	if ( (connect(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) != 0 ) {
		perror("connect()");
		return 0;
	}
	std::cout << "Connected to server!" << std::endl;


	// interact with server

	// int len = inputString.size();

	// int count = 0;
	// while ( numSend < len) {
	// 	count = send(sockfd, inputString.c_str(), len, 0);
	// 	if ( count == -1 ) {
	// 		std::cout << strerror(errno) << std::endl;
	// 		return 0;
	// 	}
	// 	numSend += count;
	// }
	// if ( (numSend = send(sockfd, inputString.c_str(), len, MSG_NOSIGNAL)) == -1 ) {
	// 	std::cout << strerror(errno) << std::endl;
	// 	return 0;
	// }
	// // print data sent by server
	// count = recv(sockfd, buffer, MAXLINE,0);
	// while ( count >= 0 ) {
	// 	numRecv += count;
	// 	if (count == 0) {
	// 		break;
	// 	}
	// }
	// if (count == -1 ) {
	// 	std::cout << strerror(errno) << std::endl;
	// 	return 0;
	// }

	int err = 0;
	// clear buffers before use
	bzero( sendBuffer, MAXLINE);
	bzero( recvBuffer, MAXLINE);

	// get string from user
	printf("\nEnter string: ");
	fgets(sendBuffer, MAXLINE,stdin);

	// send string to server 
	if ( (err = send(sockfd,sendBuffer,strlen(sendBuffer)+1,0) ) == -1 ) {
		perror("send()");
		return 0;
	}

	// read string back from server
	if ( (err = recv(sockfd,recvBuffer,MAXLINE,0)) == -1 ) {
		perror("recv()");
		return 0;
	}
	printf("From server: %s",recvBuffer);



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
			break;
		}

		// keep track of sent characters
		count += err;

		// should never happen
		if (count > length) {
			printf("\nWRITEN() ERROR!");
			break;
		}
	}

	return count;

}