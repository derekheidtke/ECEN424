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

int main(int argc, char **argv) {
	const int MAXLINE = 512;
	char buf[BUFSIZE];
	
    int servport, sockfd, data;
    int servlen;
	
    struct sockaddr_in servaddr;
    struct hostent *s_name;
	
    char const *h_name;

    if (argc != 3){
		std::cout << "Usage: server <IP_addr> <port_no>\n" << std::endl;
		return 0;
	}
	
    h_name = argv[1];
    servport = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   
   if (sockfd < 0) {
		std::cerr << "\nERROR: " << std::flush;
		std::cout << strerror(errno) << std::endl;
		return 0;
	}   

    s_name = gethostbyname(hostname);
    if (s_name == NULL) {
        std::cerr << "\nERROR: " << std::flush;
		std::cout << strerror(errno) << std::endl;
		return 0;
    }

	//Initialize servaddr to Zero
   bzero(&servaddr, sizeof(servaddr));
	
	//Create INET Address
    servaddr.sin_family = AF_INET;
    bcopy( (char const*) s_name->h_addr, &servaddr.sin_addr.s_addr, s_name->h_length);
    servaddr.sin_port = htons(servport);

	//Initialize buf to Zero
    bzero(buf, BUFSIZE);
	
	//message replace with file or ACK later
    printf("Type Outgoing Message: ");
    fgets(buf, BUFSIZE, stdin);

    //message send replace with file or ACK later
    servlen = sizeof(servaddr);
    data = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, servlen);
    if (data < 0) {
		std::cerr << "\nERROR: " << std::flush;
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
    
    //reply with file or ACK later
    data = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &servlen);
    if (data < 0) {
		std::cerr << "\nERROR: " << std::flush;
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
    printf("Echo from server: %s", buf);
    return 0;
}