// Written by Derek Heidtke

#define TFTP_RRQ 1
#define TFTP_DAT 3
#define TFTP_ACK 4
#define TFTP_ERR 5

typedef struct TFTPMESSAGE {	
	char filename[20];
	uint16_t opcode, seqNum, error;
} TFTPMESSAGE;


int tftpDecode(char* buf, TFTPMESSAGE* m) {
	uint16_t 	len = 0, i = 0, j = 0;
	char 		tmp[2];

	// memset(c,'\0',2);
	memcpy(tmp,buf,2);
	// temp = (uint16_t *)c;
	// m->opcode = ntohs(*temp);
	m->opcode = ntohs( *((uint16_t*)tmp) );
	len+=2;	
	/* Read ACK Request message*/
	if(m->opcode == TFTP_ACK)
	{
		// memset(c,'\0',2);
		memcpy(tmp,buf+len,2);
		// temp = (uint16_t *)c;
		m->seqNum = ntohs(*((uint16_t*)tmp));
		len+=2;
		
	}
	
	/* Read RRQ Request message*/
	else if(m->opcode == TFTP_RRQ)
	{
		for(i=2, j=0; buf[i]!='\0'; i++, j++)
		m->filename[j] = buf[i];
		m->filename[j] = '\0';   

		printf("%s\n",m->filename);
			
		
	}
		
	/* Read Error message*/
	else if(m->opcode == TFTP_ERR)
	{
		// memset(c,'\0',2);
		memcpy(tmp,buf+len,2);
		// temp = (uint16_t *)c;
		m->error = ntohs(*((uint16_t*)tmp));
		len+=2;
		
		
			
	}
	return 1;
}