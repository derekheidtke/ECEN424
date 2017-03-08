
struct SBCPAttribute;
struct SBCPMessage;

typedef struct SBCPAttribute{
	unsigned int	type	:2;
	unsigned int	length	:2;
	char*			payload;
} SBCPAttribute;

typedef struct SBCPMessage{
	unsigned int			vrsn	:9;
	unsigned int			type	:7;
	unsigned int			length	:2;
} SBPCMessage;


const int MESS_TYPE_JOIN	= 2;
const int MESS_TYPE_SEND	= 4;
const int MESS_TYPE_FWD		= 3;

const int ATTR_TYPE_USER	= 2;
const int ATTR_TYPE_MESS	= 4;
const int ATTR_TYPE_REAS	= 1;
const int ATTR_TYPE_CLCT	= 3;


// converts raw network traffic into SBCPMessage structure, returns the length of the entire message
int networkToStruct(char* buffer, SBCPMessage* message, SBCPAttribute* attr) {
	



}

// converts SBCPMessage to sendable network packet
// -1 on error, 0 on success
int structToNetwork(char* buffer, SBCPMessage message, SBCPAttribute* attr, int numAttr) {

	std::cerr << "AAAAAAAAAAAAAAAAAAAAA\n"<<std::flush;


	char temp[sizeof(SBCPMessage)+numAttr*sizeof(SBCPAttribute)];
	char* temp2 = (char*)attr;

	// buffer = (char*)malloc(sizeof(SBCPMessage)+numAttr*sizeof(SBCPAttribute));

	strcpy(temp,(char*)&message);
	std::cerr << "WWWWWWWWWWWWWWWWWWWW\n"<<std::flush;
	strcat(temp,temp2);

	strcpy(buffer,temp);
}