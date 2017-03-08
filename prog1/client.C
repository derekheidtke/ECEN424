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
	int sockfd, servport;
	struct sockaddr_in servaddr;
	char buffer[1024];
	
	if (argc != 3) {
		perror("Error: Input IPv4 address then socket number surrounded individually by quotations after run argument\n");
		return(-1);
	} //The running of this program should include two arguments, the IPv4 Address and the Port #. If there are not two arguments, the error is printed.
	
	servport = atoi(argv[2]); //Store the second input argument into servport as an integer.
	printf("Server Port is %d\n", servport);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); //Declare socket.
	
	if (sockfd < 0) {
		perror("Error: Could not open socket.\n");
		return(-1);
	} // sockfd returns negative if there is an error.
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(servport); //Declaring server properties.
	bzero(&servaddr, sizeof(servaddr)); //Initializing servaddr to zero.
	
	inet_pton(AF_INET, argv[1], &(servaddr.sin_addr)); //Takes input text argument and stores it in servaddr under the AF_INET format
	
	printf("%s\n", argv[1]);
	
	if (&servaddr.sin_addr <= 0) {
		perror("Error: Invalid Address.\n");
		return(-1);
	} //Check if address was entered correctly.
	
	if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		perror("Error: Socket is closed.\n");
		return(-1);
	} //Test if connection can be established.
	else {
			connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	} //Establish connection
	
	bzero(buffer, 1024); //Initialize buffer to zero.
	
	while(fgets(buffer, 1024, stdin) != NULL) { //Read from command line to the buffer continuously in 1024 segments or until an endline argument.
		
		if (write(sockfd, buffer, strlen(buffer)) < 0) {
			perror("Error: Could not write to Socket.\n");
			return(-1);
		} //Check for errors in writing to socket.
		else write(sockfd, buffer, strlen(buffer)); //Write from buffer to socket.
		
		bzero(buffer, 1024); //Flush the buffer.
		
		if (read(sockfd, buffer, 1024) < 0) {
			perror("Error: Could not read from Socket.\n");
			return(-1);
		} //Check for errors in reading server echo.
		else read(sockfd, buffer, 1024); //Read from sockfd into buffer.
		
		printf("Echo received: %s", buffer); //Print the server echo into terminal.
	}
	
	close(sockfd); //Close socket and exit program.
	exit(0);
}
