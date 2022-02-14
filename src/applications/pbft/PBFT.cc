
/*
 * PBFT.cc
 *
 *  Created on: 30/nov/2021
 *      Author: DinuFC
 */

#include <string>
#include "UnderlayConfigurator.h"
#include "GlobalStatistics.h"
#include "PBFTMessage_m.h"
#include "PBFT.h"
#include "Blockchain.h"

#define DEBUG true

Define_Module(PBFT);


void PBFT::initializeApp(int stage){
    // initializeApp will be called twice, each with a different stage.
    // stage can be either MIN_STAGE_APP (this module is being created), or MAX_STAGE_APP (all modules were created).
    // We only care about MIN_STAGE_APP here.

    if (stage != MIN_STAGE_APP) return;

    EV << "PBFT Initialization!" << endl;

    k = par("k");
    joinDelay = par("joinDelay");

    numSent = 0;
    numReceived = 0;

    WATCH(numSent);
    WATCH(numReceived);
    WATCH(k);
    WATCH(joinDelay);

    joinTimer = new cMessage("Application joinTimer");


    // bind our port to receive UDP packets
    bindToPort(2048);

    changeState(INIT);
    changeState(JOIN);
}


void PBFT::changeState(States toState){
    if (DEBUG)
        EV << "[PBFT::changeState() @ " << thisNode.getIp()
           << " New state: " << toState
           << endl;

    switch(toState){
        case INIT:
            /**
             * INIT state, some application variables are defined.
             */
            state = INIT;
            WATCH(state);

            // Assign nodeType
            nodeType = REPLICA;
            WATCH(nodeType);

            findFriendModules();
            initializeFriendModules();

            break;

        case JOIN:
            /**
             * JOIN state, the node tries to JOIN the network
             * at the application level.
             */
            state = JOIN;

            scheduleAt(simTime() + joinDelay, joinTimer);

            break;

        case READY:
            /**
             * The node is READY to work as a replica/replica+client.
             */
            state = READY;

            // TODO remove this
            // Block b = Block();
            // b.computeHash();
            break;

        case DISCONNECTED:
            /**
             * The node got notified by the overlay about disconnecting.
             */
            state = DISCONNECTED;
            break;

        case SHUTDOWN:
            /**
             * Node has to SHUTDOWN.
             */
            state = SHUTDOWN;
            break;

        case FAILED:
            /**
             * For some reason the node failed.
             */
            state = FAILED;
            break;
    }

}


void PBFT::handleTimerEvent(cMessage* msg) {
    if (msg == joinTimer) {

        /*if (underlayConfigurator->isInInitPhase()){
            EV << "UnderlayConfigurator not finished yet!" << endl;
            return;
        }
        */

        // sendMessageToLowerTier(new cPacket("Ciaoone from APP"));
        // broadcast();

        if (overlay->getState() == 6){
            // TODO A lot of other things must happen
            changeState(READY);
        } else {
            scheduleAt(simTime() + joinDelay, joinTimer);
        }

    } else {
        delete msg; // unknown packet
    }
}


void PBFT::findFriendModules(){
    chainModule = check_and_cast<Blockchain*> (getParentModule()->getSubmodule("chain"));
}

void PBFT::initializeFriendModules(){
    chainModule->initializeChain();

}


// deliver is called when we receive a message from the overlay.
// TODO This method will never be called in my application
void PBFT::deliver(OverlayKey& key, cMessage* msg) {
    if(DEBUG)
        EV << "[PBFT::deliver() @ " << thisNode.getIp()
           << "Delivered a message"
           << endl;
    // PBFTMessage *myMsg = dynamic_cast<PBFTMessage*>(msg);

    delete msg;
}

// handleUDPMessage is called when we receive a message from UDP.
// Parameter msg is actually of type cPacket*, set to the more generic cMessage* for legacy purposes.

void PBFT::handleUDPMessage(cMessage* msg) {
    if(DEBUG)
        EV << "[PBFT::handleUDPMessage() @ " << thisNode.getIp()
           << " Received a message"
           << endl;

    // PBFTMessage *myMsg = dynamic_cast<PBFTMessage*>(msg);

    delete msg;
}

void PBFT::update(const NodeHandle& node, bool joined){
    if(DEBUG)
        EV << "[PBFT::update() @ " << thisNode.getIp()
           << " Received a notification about my neighbors from the overlay\n"
           << node.getIp() << " is my neighbor: " << joined
           << endl;

    // TODO Understand what are the next steps eventually

}

void PBFT::broadcast(){
    if(DEBUG)
        EV << "[PBFT::broadcast() @ " << thisNode.getIp()
           << endl;

    NodeVector* nodes = callNeighborSet(k);
    for(int i=0; i<(int)nodes->size(); i++){
        // send UDP message
        PBFTMessage* msg = new PBFTMessage("PBFTMessage");

        nodes->at(i).setPort(2048);
        sendMessageToUDP(nodes->at(i), msg);
    }
}

void PBFT::handleLowerMessage(cMessage* msg){
    if(DEBUG)
        EV << "[PBFT::handleLowerMessage() @ " << thisNode.getIp()
           << endl;

    delete msg;
}

void PBFT::finishApp() {
    delete joinTimer;

    globalStatistics->addStdDev("PBFT: Sent packets", numSent);
    globalStatistics->addStdDev("PBFT: Received packets", numReceived);
}


