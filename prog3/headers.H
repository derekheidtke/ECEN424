// Written by Derek Heidtke

#define TFTP_RRQ 1
#define TFTP_DAT 3
#define TFTP_ACK 4
#define TFTP_ERR 5

#define MAXLINE 512

typedef struct TFTPMESSAGE {	
	char filename[20];
	uint16_t opcode, seqNum, error;
} TFTPMESSAGE;

typedef struct CLIRECORD
{
	int 		n;
	uint16_t	seqNum;
	int 		cliSockfd;
	FILE 		*fp1, *fp2;
	char 		filename[20];
	struct 		timespec time;
	int 		lastSegmentSent;
	struct 		sockaddr_in cliaddr;
} CLIRECORD;

// translate/decode/copy incoming message from buffer to TFTPMESSAGE object
int tftpDecode(char* buf, TFTPMESSAGE* m) {

	uint16_t 	offset, i, j;
	char 		tmp[2];

	offset = 0;
	i = 0;
	j = 0;

	memcpy(tmp, buf, 2);
	m->opcode = ntohs( *((uint16_t*)tmp) );
	offset += 2;	

	if(m->opcode == TFTP_ACK) {				// if ACK message
		memcpy(tmp, buf+offset, 2);
		m->seqNum = ntohs(*((uint16_t*)tmp));
		offset += 2;
		
	} else if(m->opcode == TFTP_RRQ) {		// if RRQ message
		for(i=2, j=0; buf[i] != '\0'; i++, j++){
			m->filename[j] = buf[i];
		}
		m->filename[j] = '\0';

		// std::cout << "\n" << m->filename << std::flush;

	} else if(m->opcode == TFTP_ERR) {		// if ERR message
		memcpy(tmp, buf+offset, 2);
		m->error = ntohs(*((uint16_t*)tmp));
		offset += 2;	
	}

	return 0;
}

void make_tftp_message(int len, FILE* fp, char *&buffer, uint16_t opcode, uint16_t seqNum) {
	
	// size += len;
	uint16_t size = 4 + len;

	// allocate new space for data
	buffer = new char[size];
	memset(buffer,'\0',size);

	// insert first two data fields: opcode, blcok number
	opcode = htons(opcode);
	seqNum = htons(seqNum);
	memcpy(buffer, &opcode,2);
	memcpy(buffer+2, &seqNum,2);

	// append file data to data packet
	fread(buffer+4,1,len, fp);	
}

// send specified data file to specified client
int tftpRespondRRQ(TFTPMESSAGE* m, struct sockaddr* pcliaddr, CLIRECORD* clientInfo) {

	int 			n;
	std::string		bufstr;

	std::cout << "\nSending file: <" << m->filename << "> to client." << std::flush;

	// open the file from the beginning
	clientInfo->fp1 = fopen(m->filename, "r");
	if (clientInfo->fp1 != NULL) {	// file opened successfully
		
		// start constructing the data to be sent
		TFTPMESSAGE sendData;
		sendData.seqNum = 1;
		sendData.opcode = TFTP_DAT;
		std::cout << "\nFile opened successfully." << std::flush;

		// open file from the end
		clientInfo->fp2 = fopen(m->filename, "r");
		if (clientInfo->fp2 == NULL) {
			std::cerr << "\nERROR: " << strerror(errno) << std::endl;
			return 0;
		}
		fseek(clientInfo->fp2, 0, SEEK_END);

		// find the difference
		n = ftell(clientInfo->fp2) - ftell(clientInfo->fp1);

		// updata clientInfo
		if (n >= MAXLINE) {
			n = MAXLINE;
		} else {
			clientInfo->lastSegmentSent = 1;
		}
		clientInfo->n = n;

		// // finish making the message
		char* 	buf;

		make_tftp_message(n, clientInfo->fp1, buf, sendData.opcode, sendData.seqNum);

		// std::cout << "\nsendBuf: " << buf << std::flush;

		// send the message
		sendto(clientInfo->cliSockfd, buf, n+4, 0, pcliaddr, sizeof(struct sockaddr_in) );
		std::cout << "\nSending segment of size, " << n << " to client." << std::flush;

	} else { // error opening file

		std::cout << "\nCould not open file." << std::flush;

		/* creat error message*/
		uint16_t 	opcode 	= TFTP_ERR;
		uint16_t 	errcode = 1;
		int 		errSize = 100;

		// allocate space for error message
		char buffer[errSize];
		memset(buffer,'\0',errSize);

		// fill data fields with opcode, error code, and error message
		opcode = htons(opcode);
		errcode = htons(errcode);
		memcpy(buffer, &opcode, 2);
		memcpy(buffer+2, &errcode, 2);
		memcpy(buffer+4,"File not Found",strlen("File not Found"));

		sendto(clientInfo->cliSockfd, buffer, errSize, 0, pcliaddr, sizeof(struct sockaddr_in) );

		return -1; 		
	}

	return 0;
}

int tftpRespondACK(TFTPMESSAGE* m, struct sockaddr* pcliaddr, CLIRECORD* clientInfo) {

	int 	n;

	std::cout << "\nReceived ACK for block number: " << m->seqNum << std::flush;

	// update clientInfo
	clientInfo->seqNum = m->seqNum + 1;	// the NEXT data block

	// start constructing the data to be sent
	TFTPMESSAGE sendData;
	sendData.seqNum = clientInfo->seqNum;
	sendData.opcode = TFTP_DAT;

	// open file from the end
	clientInfo->fp2 = fopen(m->filename, "r");
	if (clientInfo->fp2 == NULL) {
		std::cerr << "\nERROR: " << strerror(errno) << std::endl;
		return 0;
	}
	fseek(clientInfo->fp2, 0, SEEK_END);

	// find the difference
	n = ftell(clientInfo->fp2) - ftell(clientInfo->fp1);

	// updata clientInfo
	if (n >= MAXLINE) {
		n = MAXLINE;
	} else {
		clientInfo->lastSegmentSent = 1;
	}
	clientInfo->n = n;

	// finish making the message
	char*	buf;
	make_tftp_message(n, clientInfo->fp1, buf, sendData.opcode, sendData.seqNum);

	// send the message
	sendto(clientInfo->cliSockfd, buf, n+4, 0, pcliaddr, sizeof(struct sockaddr_in) );
	std::cout << " >> Sending block number, " << sendData.seqNum << " ..." << std::flush;
	
	return 0;
}