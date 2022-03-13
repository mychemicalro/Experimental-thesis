


cplusplus {{
#include <IPvXAddress.h>
#include <NodeHandle.h>
#include <TransportAddress.h>
#include <CommonMessages_m.h>
#include <OverlayKey.h>

// constants for message length in bit
static const int SCAMPCOMMAND_L = 4;
static const int STEP_L = 8;
static const int SIMTIME_L = 4;

#define JOINCALL_L(msg) (BASECALL_L(msg) + IPADDR_L + SIMTIME_L)
#define JOINRESPONSE_L(msg) (BASERESPONSE_L(msg) + SCAMPCOMMAND_L)

#define SCAMP_MESSAGE_L(msg) (BASECALL_L(msg) + SCAMPCOMMAND_L + NODEHANDLE_L)
#define FORWARD_MESSAGE_L(msg) (SCAMP_MESSAGE_L(msg) + STEP_L + NODEHANDLE_L)
#define LEAVE_MESSAGE_L(msg) (SCAMP_MESSAGE_L(msg) + NODEHANDLE_L)
#define NEW_NODE_L(msg) (SCAMP_MESSAGE_L(msg))

}}


class noncobject IPvXAddress;
class noncobject NodeHandle;
class noncobject OverlayKey;
class noncobject TransportAddress;
class BaseOverlayMessage;
class BaseRpcMessage;
class BaseCallMessage;
class BaseResponseMessage;

enum ScampCommand
{
    JOINREQUESTACCEPTED = 0;
    JOINREQUESTFORWARDED = 1;
    ADDREQUESTACCEPTED = 2;
    ADDREQUESTDENIED = 3;
    JOINREQUESTENDED = 4;
    ADDEDTOPARTIALVIEW = 5;
    HEARTBEAT = 6;
    REPLACE_ME = 7;
    REMOVE_ME = 8;
    NEW_NODE = 9;
    HEARTBEAT_FROM_LEAF = 10;
}

//
// Base message for all messages used by Scamp
//
packet ScampMessage extends BaseOverlayMessage{
    int command enum(ScampCommand);  
    NodeHandle node;  
}

packet ForwardMessage extends ScampMessage{
	int steps;
	NodeHandle sourceNode;
}

packet LeaveMessage extends ScampMessage{
    // the node will be the requester
    NodeHandle newNode; // this will be the new NodeHandle to replace with 
}

packet NewNodeMessage extends ScampMessage{
}

//
// Message used to join a scamp overlay
//
packet ScampJoinCall extends BaseCallMessage{
    IPvXAddress address;
    simtime_t time;
}

//
// Message used to answer to a ScampJoinCall message
//
packet ScampJoinResponse extends BaseResponseMessage{
    int command enum(ScampCommand);
}







