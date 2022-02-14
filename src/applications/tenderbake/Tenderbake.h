#ifndef TENDERBAKE_H
#define TENDERBAKE_H

#include <omnetpp.h>
#include "BaseApp.h"

class Blockchain;

class Tenderbake : public BaseApp {

    // module parameters
    int k;
    double joinDelay;

    // statistics
    int numSent;              //number of packets sent
    int numReceived;          //number of packets received

    // our timer
    cMessage *joinTimer;

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
    void broadcast();

    /**
     * Handles messages received from the overlay
     */
    void handleLowerMessage(cMessage* msg);




public:
    Tenderbake() {
        joinTimer = NULL;
        chainModule = NULL;
    };

    ~Tenderbake() {
        // cancelAndDelete(timerMsg);

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


protected:
    // Class attributes
    States state;
    NodeTypes nodeType;
    Blockchain* chainModule;
};


#endif
