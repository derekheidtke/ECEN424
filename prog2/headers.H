
struct SBCPAttribute;
struct SBCPMessage;

typedef struct SBCPAttribute{
	uint8_t		type	:2;
	uint8_t		length	:2;
	char*		payload;
} SBCPAttribute;


typedef struct SBCPMessage{
	uint16_t	vrsn	:9;
	uint8_t		type	:7;
	uint8_t		length	:2;
} SBPCMessage;

static inline void createMess (SBCPMessage* message,int vrsn, int type, int length) {
	message->vrsn 	= (vrsn & 0x1FF);
	message->type 	= (type & 0x7F);
	message->length = (length & 0x3);
}

static inline void createAttr (SBCPAttribute* attr, int type, int length) {
	attr->type 		= (type & 0x3);
	attr->length 	= (length & 0x3);
}


const int MESS_TYPE_JOIN	= 2;
const int MESS_TYPE_SEND	= 4;
const int MESS_TYPE_FWD		= 3;

const int ATTR_TYPE_USER	= 2;
const int ATTR_TYPE_MESS	= 4;
const int ATTR_TYPE_REAS	= 1;
const int ATTR_TYPE_CLCT	= 3;


// converts raw network traffic into SBCPMessage structure, returns the length of the entire message
int deserializePacket(uint8_t* buffer, SBCPMessage* message, SBCPAttribute* attrList) {

	// Read outer layer of message
	message = (SBCPMessage*)buffer;


}

// converts SBCPMessage to sendable network packet.
// returns pointer to next available free byte, NULL on error
uint8_t* serializePacket(uint8_t* buffer, int MAXLINE, SBCPMessage message, SBCPAttribute* attr, int numAttr) {

	// reset the buffer
	bzero(buffer,MAXLINE);

	uint8_t*	temp;
	uint8_t*	end = buffer;

	// construct text string from structure contents
	// message is 4 bytes
	temp = (uint8_t*)&message;
	*end = temp[0]; end++;
	*end = temp[1]; end++;
	*end = temp[2]; end++;
	*end = temp[3]; end++;


	// uint8_t temp[sizeof(SBCPMessage)+numAttr*sizeof(SBCPAttribute)];
	// uint8_t* temp2 = (uint8_t*)attr;
	// strcpy(temp,(uint8_t*)&message);
	// strcat(temp,temp2);


	// add attributes to the buffer
	for (int i = 0; i<numAttr; i++) {

	}

	// copy from temporary string to buffer provided by caller
	// strcpy((char*)buffer,(char*)temp);

	return end;
}

void printMessage (SBCPMessage* message) {
	std::cout << "\nMESSAGE:\nvrsn: " << message->vrsn << "\ntype: " << message->type << "\nlength: " << message->length << std::endl;
}