#include <stdio.h>
#include <stdlib.h>
#include <errno.h>					// defines perror(), herror() 
#include <fcntl.h>					// set socket to non-blocking with fcntrl()
#include <unistd.h>
#include <string.h>
#include <assert.h>					//add diagnostics to a program

#include <netinet/in.h>			//defines in_addr and sockaddr_in structures
#include <arpa/inet.h>			//external definitions for inet functions
#include <netdb.h>					//getaddrinfo() & gethostbyname()

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/select.h>			// for select() system call only
#include <sys/time.h>				// time() & clock_gettime()


int main (int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;
	char cli_msg[1024];
	char buffer[1024];
	FILE * fp;
	
	if (argc != 2) {
		printf("Error: Address currently in use.\n");
		return(-1);
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0) {
		printf("Error: Could not open socket.\n");
		return(-1);
	}
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(12);
	bzero(&servaddr, sizeof(servaddr));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		printf("Error: Socket is closed.\n");
		return(-1);
	}
	else {
			connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	}
	
	bzero(buffer, 1024);
	
	 if (fp == NULL) {
		 printf("Error: Could not open file.\n");
		 return(-1);
	}
	
	while(fgets(cli_msg, 1024, fp) != NULL) {
		if (write(sockfd, buffer, strlen(buffer)) < 0) {
			printf("Error: Could not write to Socket.\n");
			return(-1);
		}
		else write(sockfd, buffer, strlen(buffer));
		
		bzero(buffer, 1024);
		
		if (read(sockfd, buffer, 1024) < 0) {
			printf("Error: Could not read from Socket.\n");
			return(-1);
		}
		else read(sockfd, buffer, 1024);
		
		printf("Echo received: %s", buffer);
	}
	
	fclose(sockfd);
	exit(0);
}
