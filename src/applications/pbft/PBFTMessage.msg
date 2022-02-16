
// Import the C++ TransportAddress class

cplusplus {{
#include <TransportAddress.h>
#include <OverlayKey.h>
#include <Operation.h>
}}
class noncobject TransportAddress;            // noncobject means it's not a message class.
class noncobject OverlayKey;            	  // noncobject means it's not a message class.
class noncobject Operation;            		  // noncobject means it's not a message class.

// First, we declare an enum for the message type:
// Enumerations in messages MUST have a value.

enum MessageType {
    REQUEST = 1;
	PREPREPARE = 2;
	PREPARE = 3;
	COMMIT = 4;
	REPLY = 5;
}

// now we declare the message proper

packet PBFTMessage {
    int type enum (MessageType);     // message type
    simtime_t timestamp; 			 // timestamp when the messages was generated.
    
    // TransportAddress senderAddress;  // address of the node that sent the message
    // OverlayKey senderKey;				 // overlayKey of the client that sent the message
    
}

packet PBFTRequestMessage extends PBFTMessage {
	type = REQUEST;
	Operation op;
	TransportAddress clientAddress;
	OverlayKey clientKey;
}

packet PBFTPreprepareMessage extends PBFTMessage {
	type = PREPREPARE;
}

packet PBFTPrepareMessage extends PBFTMessage {
	type = PREPARE;
}

packet PBFTCommitMessage extends PBFTMessage {
	type = COMMIT;
}

packet PBFTReplyMessage extends PBFTMessage {
	type = REPLY;
	int view;
	OverlayKey replicaNumber; 							// replica number
	int operationResult; 								// the result of the operation TODO
}





