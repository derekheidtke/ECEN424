// Chat Client
// Written by: Ryan Hill

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "headers.h"

int writen(int, char*, int);
int readline(int , char*, int );

int main (int argc, char** argv) {

	int 				SERVPORT, sockfd;//, serverfd;
	int numSend = 0;
	int numRecv = 0;
	
	int i;

	struct sockaddr_in 	serv_addr;
	char*				servAddrsString;
	char*				username;
	
	ssize_t n;
	fd_set rset,allset;
	
	std::string 		inputString;

	size_t 				MAXLINE = 255;
	char 				sendBuffer[MAXLINE];
	char 				recvBuffer[MAXLINE];
	uint8_t			buffer[MAXLINE];
	
	SBCPMessage			message;
 	SBCPAttribute*			attrList[16] = {0};

	// Get IP addr and port number of server from command line
	if (argc != 4){
		std::cout << "Usage: echos [IPv4_addr] [port_no]\n" << std::endl;
		return -1;
	}

	servAddrsString = argv[1];
	std::cout << "IP_addr: " << servAddrsString << std::endl;

	SERVPORT = atoi(argv[2]);
	std::cout << "Port_Num: " << SERVPORT << std::endl;
	
	username = argv[3];
	std::cout << "Username: " << username << std::endl;

	// Preliminary stuff
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;				// use IPv4 protocol
	serv_addr.sin_port 			= htons(SERVPORT);		// use portno provided by user
	
	if ( inet_pton(AF_INET, servAddrsString, &serv_addr.sin_addr) != 1 ) {
		perror("inet_pton()");
		return -1;
	}

	// Create socket
	if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
		perror("socket()");
		return -1;
	}
	std::cout << "Socket created successfully." << std::endl;

	// Connect to server
	if ( (connect(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) != 0 ) {
		perror("connect()");
		return -1;
	}
	std::cout << "Connected to server!" << std::endl;

	// interact with server
	
	// start JOIN
    message.vrsn=3;
	message.type=MESS_TYPE_JOIN;
	message.length=8;
	createMess(&message,3,MESS_TYPE_JOIN,8);
	
	attrList = (SBCPAttribute*)malloc(sizeof(SBCPAttribute));
	createAttr(&attrList[0],ATTR_TYPE_USER,8);
	attrList[0].type = ATTR_TYPE_USER;
 	attrList[0].payload = username;
	attrList[0].length = 8;

    // change SBCP Message to Network Packet
	serializePacket(buffer, MAXLINE, message,attrList,1);
	
	// Send Username Data
	write(sockfd, buffer, MAXLINE);
	
	// Reset Data Structures
	bzero(buffer,MAXLINE);
	free(attrList);

	// prepare data of chat message
    message.vrsn=3;
	message.type=MESS_TYPE_SEND;
	message.length=520;
	createMess(&message,3,MESS_TYPE_SEND,520);
	
	attrList = (SBCPAttribute*)malloc(sizeof(SBCPAttribute));
	createAttr(&attrList[0],ATTR_TYPE_MESS,520);
	char temp[520] = "default message";
	attrList[0].type = ATTR_TYPE_USER;
 	attrList[0].payload = temp;
	attrList[0].length = 8;

	FD_SET(0, &rset); // add the keyboard input to the read_fds set
	FD_SET(sockfd, &rset);// add the sockfd to the read_fds set

	for(;;) {	
		if (select(sockfd+1, &rset, NULL, NULL, NULL) == -1) {
			perror("Select Error");
			return -1;
		}
		for(i = 0; i <= sockfd; i++) {																					// find if there is data to read from server or keyboard
			if (FD_ISSET(i, &rset)) {
				if (i == 0) {																														//there is data to read from keyboard
					fgets(sendBuffer, MAXLINE, stdin);
	                if (sendBuffer[MAXLINE-1] == '\n') sendBuffer[MAXLINE-1] = '\0';					// chage data to network byte order
					if (writen(sockfd, sendBuffer, MAXLINE) == -1) {
						perror("Send Error");
						return -1;
					}
				}
				if (i == sockfd){																//there is data to read from server
					if ( (n = recv(sockfd, recvBuffer, MAXLINE,0) ) <= 0 ) {
						// close sockfd
						FD_CLR(sockfd, &allset);
						return -1;
					} else {
						// get the new data from server
						deserializePacket(recvbuffer, &message, attrList);
					}
					recvbuffer[n] = '\0';
					printf("%s\n",recvbuffer);
				}
				
			}
			
			FD_SET(0, &rset);
	        FD_SET(sockfd, &rset);
		}
	}
	close(sockfd);

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
int readline(int sockfd, char* buffer, int bufferlength) {

	int		err = 0;
	int		count = 0;	// keep track of how many chars the client sent to the server

	while ( count < bufferlength ) {

		err = recv(sockfd, buffer, bufferlength, 0);

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
		if (count > bufferlength) {
			printf("\nREADLINE() ERROR!");
			break;
		}
	}

	return count;
}