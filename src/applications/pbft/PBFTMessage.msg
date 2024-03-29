
// Import the C++ TransportAddress class

cplusplus {{
#include <TransportAddress.h>
#include <OverlayKey.h>
#include <Operation.h>
#include <Block.h>


// constants for message length in bit
static const int MESSAGETYPE_L = 3;
static const int OPERATION_L = 208; // TODO Count the bits of the class fields 2 strings (128) + timestamp(16) + pair overlaykey(32) + ipaddress(32)
static const int VIEW_L = 8;
static const int SEQNUMBER_L = 8;
static const int DIGEST_L = 64; 
static const int OPRESULT_L = 1;
static const int REPLICANUMBER_L = 32;
static const int CREATORADDRESS_L = 32; 
static const int CREATORKEY_L = 32;
static const int BLOCK_L = 88; // TODO strings(64) + 3 int (24) + 208*size()op
static const int RETRYNUMBER_L = 8;


#define PBFTMESSAGE_L(msg) (BASECALL_L(msg) + MESSAGETYPE_L + RETRYNUMBER_L + TRANSPORTADDRESS_L + KEY_L)
#define PBFTREQUEST(msg) (PBFTMESSAGE_L(msg) + OPERATION_L)
#define PBFTPREPREPARE(msg) (PBFTMESSAGE_L(msg) + VIEW_L + SEQNUMBER_L + DIGEST_L + OPERATION_L * msg->getBlock().getBlockOpsNumber())
#define PBFTPREPARE(msg) (PBFTMESSAGE_L(msg) + VIEW_L + SEQNUMBER_L + DIGEST_L)
#define PBFTCOMMIT(msg) (PBFTMESSAGE_L(msg) + VIEW_L + SEQNUMBER_L + DIGEST_L)
#define PBFTREPLY(msg) (PBFTMESSAGE_L(msg) + VIEW_L + OPRESULT_L + OPERATION_L + KEY_L)
#define PBFTCHECKPOINT(msg) (PBFTMESSAGE_L(msg) + DIGEST_L + SEQNUMBER_L)
#define PBFTUPDATE(msg) (PBFTMESSAGE_L(msg) + 6 * SEQNUMBER_L)
#define PBFTUPDATEREQUEST(mgs)  (PBFTMESSAGE_L(msg))

}}
class noncobject TransportAddress;            // noncobject means it's not a message class.
class noncobject OverlayKey;            	  // noncobject means it's not a message class.
class noncobject Operation;            		  // noncobject means it's not a message class.
class noncobject Block;            		      // noncobject means it's not a message class.

// First, we declare an enum for the message type:
// Enumerations in messages MUST have a value.

enum MessageType {
    REQUEST = 1;
	PREPREPARE = 2;
	PREPARE = 3;
	COMMIT = 4;
	REPLY = 5;
	CHECKPOINT = 6;
	UPDATE = 7;
	UPDATE_REQUEST = 8;
}

// now we declare the message proper

packet PBFTMessage {
    int type enum (MessageType);         // message type
    int retryNumber = 0;				 // acts like a sequence number for all messages
  
	TransportAddress creatorAddress;      // address of the node that created the message
    OverlayKey creatorKey;				 // overlayKey of the node that created the message

}

packet PBFTRequestMessage extends PBFTMessage {
	type = REQUEST;
	Operation op;
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
}

packet PBFTCommitMessage extends PBFTMessage {
	type = COMMIT;
	int view;
	int seqNumber;
	string digest;
}

packet PBFTReplyMessage extends PBFTMessage {
	type = REPLY;
	int view;
	OverlayKey replicaNumber; 							// replica number
	Block block;
}

packet PBFTCheckpointMessage extends PBFTMessage {
    type = CHECKPOINT;
    int seqNumber;
    string digest;
}

packet PBFTUpdateMessage extends PBFTMessage {
    type = UPDATE;
    int seqNumber;
    int view;
    int lowH;
    int highH;
    int blockchain_length;
    int blockchainModuleId;
}

packet PBFTUpdateRequestMessage extends PBFTMessage {
    type = UPDATE_REQUEST;
}




