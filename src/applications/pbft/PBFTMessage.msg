
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

enum MessageType 
{
    REQUEST = 1;           // outgoing message
}

// now we declare the message proper

packet PBFTMessage 
{
    int type enum (MessageType);     // message type
    TransportAddress senderAddress;  // address of the node that sent the message
    OverlayKey oKey;				 // overlayKey of the node that sent the message
    simtime_t timestamp; 			 // timestamp when the messages was generated 
    Operation op;
}