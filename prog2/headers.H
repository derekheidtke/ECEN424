// Written by Derek Heidtke

struct SBCPAttribute;
struct SBCPMessage;

typedef struct SBCPAttribute{
	uint16_t		type	:16;
	uint16_t		length	:16;
	char* 			payload;
} SBCPAttribute;


typedef struct SBCPMessage{
	uint16_t		vrsn	:9;
	uint16_t		type	:7;
	uint16_t		length	:16;
} SBPCMessage;

static inline void createMess (SBCPMessage* message,int vrsn, int type, int length) {
	message->vrsn 	= (vrsn & 0x1FF);
	message->type 	= (type & 0x7F);
	message->length = (length & 0xFFFF);
}

static inline void createAttr (SBCPAttribute* attr, int type, int length) {
	attr->type 		= (type & 0xFFFF);
	attr->length 	= (length & 0xFFFF);
}


const int MESS_TYPE_JOIN	= 2;
const int MESS_TYPE_SEND	= 4;
const int MESS_TYPE_FWD		= 3;

const int ATTR_TYPE_USER	= 2;
const int ATTR_TYPE_MESS	= 4;
const int ATTR_TYPE_REAS	= 1;
const int ATTR_TYPE_CLCT	= 3;


// converts raw network traffic into SBCPMessage structure, returns the length of the entire message
int deserializePacket(uint8_t* buffer, SBCPMessage* message, SBCPAttribute** attrList) {


	// number of bytes left to be read
	int remainingLen = message->length;
	// next  = address of next byte to be read
	uint8_t* 	next = (uint8_t*)message;

	int currentAttr = -1;

	// Read outer layer of message (guaranteed 4 bytes)
	*next = buffer[0]; next++; remainingLen--;
	*next = buffer[1]; next++; remainingLen--;
	*next = buffer[2]; next++; remainingLen--;
	*next = buffer[3]; next++; remainingLen--;

	// processing one or more attributes
	while (remainingLen > 0) {

		currentAttr++;

		// read 4 bytes from next attribute
		attrList[currentAttr] = (SBCPAttribute*)next;

		

	}


}

// converts SBCPMessage to sendable network packet.
// returns pointer to next available free byte, NULL on error
uint8_t* serializePacket(uint8_t* buffer, int MAXLINE, SBCPMessage message, SBCPAttribute** attrList, int numAttr) {

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

	// add attributes to the buffer
	for (int i = 0; i<numAttr; i++) {

		temp = (uint8_t*)attrList[i];

		// append the ith attribute header
		*end = temp[0]; end++;
		*end = temp[1]; end++;
		*end = temp[2]; end++;
		*end = temp[3]; end++;

		temp = (uint8_t*)(attrList[i]->payload);

		for (int k = 0; k<(attrList[i]->length); k++ ){
			*end = temp[k]; end++;
		}
		// *end = '\0'; end++;
	}

	return end;
}

void printMessage (SBCPMessage* message) {
	std::cout << "\nMESSAGE:\nvrsn: " << message->vrsn << "\ntype: " << message->type << "\nlength: " << message->length << std::endl;
}

void printAttribute (SBCPAttribute* attr) {
	std::cout << "\nMESSAGE:\ntype: " << attr->type << "\nlength: " << attr->length << "\npayload: "<< attr->payload << std::endl;
}