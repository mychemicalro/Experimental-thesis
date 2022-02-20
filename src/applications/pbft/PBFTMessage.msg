
// Import the C++ TransportAddress class

cplusplus {{
#include <TransportAddress.h>
#include <OverlayKey.h>
#include <Operation.h>
#include <Block.h>
}}
class noncobject TransportAddress;            // noncobject means it's not a message class.
class noncobject OverlayKey;            	  // noncobject means it's not a message class.
class noncobject Operation;            		  // noncobject means it's not a message class.
class noncobject Block;            		  // noncobject means it's not a message class.

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
    int type enum (MessageType);         // message type

	// These are envelop data TODO understand if these data are necessary      
    TransportAddress senderAddress;      // address of the node that sent the message
    OverlayKey senderKey;				 // overlayKey of the node that sent the message
    
}

packet PBFTRequestMessage extends PBFTMessage {
	type = REQUEST;
	Operation op;
	TransportAddress clientAddress;
	OverlayKey clientKey;
	simtime_t timestamp; 
}

packet PBFTPreprepareMessage extends PBFTMessage {
	type = PREPREPARE;
	int view;
	int seqNumber;
	string digest; // Block digest
	Block block; // block containing the operations and also view and sequence number
}

packet PBFTPrepareMessage extends PBFTMessage {
	type = PREPARE;
	int view;
	int seqNumber;
	string digest;
	TransportAddress creatorAddress;      // address of the node that created the PREPARE message
    OverlayKey creatorKey;				 // overlayKey of the node that created the PREPARE message
}

packet PBFTCommitMessage extends PBFTMessage {
	type = COMMIT;
	int view;
	int seqNumber;
	string digest;
	TransportAddress creatorAddress;      // address of the node that created the PREPARE message
    OverlayKey creatorKey;				 // overlayKey of the node that created the PREPARE message
}

packet PBFTReplyMessage extends PBFTMessage {
	type = REPLY;
	int view;
	OverlayKey replicaNumber; 							// replica number
	int operationResult; 								// the result of the operation TODO
}





