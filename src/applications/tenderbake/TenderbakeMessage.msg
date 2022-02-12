
// Import the C++ TransportAddress class

cplusplus {{
#include <TransportAddress.h>
}}
class noncobject TransportAddress;            // noncobject means it's not a message class.

// First, we declare an enum for the message type:
// Enumerations in messages MUST have a value.

enum MessageType 
{
    MYMSG_PING = 1;           // outgoing message
    MYMSG_PONG = 2;           // returning message
}

// now we declare the message proper

packet TenderbakeMessage 
{
    int type enum (MessageType);     // message type
    TransportAddress senderAddress;  // address of the node that sent the message
}