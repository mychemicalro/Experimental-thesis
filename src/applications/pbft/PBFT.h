#ifndef PBFT_H
#define PBFT_H

#include <omnetpp.h>
#include "BaseApp.h"
#include "Block.h"

class Blockchain;
class ReplicaState;

class PBFT : public BaseApp {

    // module parameters
    int k;
    double joinDelay;
    double requestDelay;
    double replyDelay;

    // statistics
    int numSent;              //number of packets sent
    int numReceived;          //number of packets received

    // our timer
    cMessage *joinTimer;
    cMessage *clientTimer;  // timer to send messages as a client
    cMessage *replyTimer;  // timer after which a client resends the request

    PBFTRequestMessage* actualRequest;

    // application routines
    void initializeApp(int stage);                 // called when the module is being created
    void finishApp();                              // called when the module is about to be destroyed
    void handleTimerEvent(cMessage* msg);          // called when we received a timer message
    void deliver(OverlayKey& key, cMessage* msg);  // called when we receive a message from the overlay
    void handleUDPMessage(cMessage* msg);          // called when we receive a UDP message

    virtual void update(const NodeHandle& node, bool joined);

    /**
     * Gossips a message. It will send a message to k nodes.
     * k is an application parameter.
     */
    void broadcast(cMessage* msg);

    /**
     * Handles messages received from the overlay
     */
    void handleLowerMessage(cMessage* msg);

    /**
     * Returns true if the node is the primary for the current view
     * TODO
     */
    bool isPrimary();

    /**
     * Send an UDP message only to my node
     */
    void sendToMyNode(cMessage* msg);

public:
    PBFT() {
        joinTimer = NULL;
        clientTimer = NULL;
        chainModule = NULL;
        replicaStateModule = NULL;

    };

    ~PBFT() {
        cancelAndDelete(joinTimer);
        cancelAndDelete(clientTimer);

    };

    enum States {
        INIT      = 1,
        JOIN      = 2,
        READY     = 3,
        SHUTDOWN  = 4,
        FAILED    = 5,
        DISCONNECTED = 6,
    };

    enum NodeTypes {
        REPLICA = 1,
        REPLICAANDCLIENT = 2,
    };

    enum OpResults{
        OK = 1,
        KO = 0,
    };


    States getState() { return state; };
    NodeTypes getNodeType(){ return nodeType; }

    /**
     * Handles the state transitions of the application
     */
    void changeState(States toState);

    /**
     * Method used to find the Blockchain module
     */
    void findFriendModules();

    /**
     * Call initializeChain in Blockchain module
     */
    void initializeFriendModules();

    void handleRequestMessage(cMessage* msg);


    void handlePrepareMessage(cMessage* msg);

    void handlePreprepareMessage(cMessage* msg);


    void handleCommitMessage(cMessage* msg);

    void handleReplyMessage(cMessage* msg);


    void onDemandPrePrepare(PBFTRequestMessage* req);


protected:
    // Class attributes
    States state;
    NodeTypes nodeType;
    Blockchain* chainModule;
    ReplicaState* replicaStateModule;

    int h; // low watermark
    int H; // high watermark
    int checkpointPeriod;
    double clientProb;

    // Algorithm attributes
    int sequenceNumber;

    int blockCapacity;
    Block* nextBlock;

};


#endif
