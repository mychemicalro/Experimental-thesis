
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
#include "ReplicaState.h"

#define DEBUG true

Define_Module(PBFT);


void PBFT::initializeApp(int stage){
    // initializeApp will be called twice, each with a different stage.
    // stage can be either MIN_STAGE_APP (this module is being created), or MAX_STAGE_APP (all modules were created).
    // We only care about MIN_STAGE_APP here.

    if (stage != MIN_STAGE_APP) return;

    k = par("k");
    joinDelay = par("joinDelay");

    numSent = 0;
    numReceived = 0;

    sequenceNumber = 0;

    WATCH(numSent);
    WATCH(numReceived);
    WATCH(k);
    WATCH(joinDelay);
    WATCH(sequenceNumber);

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

            // Here I do not have yet an overlayKey

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

            // Let's make this a client TODO This will have to be removed
            if (this->overlay->getThisNode().getKey() < OverlayKey(2)){
                nodeType = REPLICAANDCLIENT;

                // Set timer for sending messages
                clientTimer = new cMessage("Client timer");

                // TODO add some delay parameter
                scheduleAt(simTime() + uniform(1,3), clientTimer);
            }
            // TODO solve cancelAndDelete(joinTimer) issue in the destructor of PBFT

            // Check if this is the primary, and set the state accordingly.
            isPrimary();

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

        if (overlay->getState() == 6){
            // TODO A lot of other things must happen
            changeState(READY);
        } else {
            scheduleAt(simTime() + joinDelay, joinTimer);
        }

    } else if (msg == clientTimer){
        // Create a new operation and broadcast it

        Operation op = Operation(this->overlay->getThisNode().getKey(), thisNode.getIp(), simTime());
        PBFTRequestMessage* msg = new PBFTRequestMessage("PBFTMessage");
        msg->setOp(op);
        msg->setTimestamp(simTime()); // TODO not always needed
        msg->setClientKey(this->overlay->getThisNode().getKey());
        msg->setClientAddress(thisNode); // This includes also the port, like 10.0.0.1:2048

        broadcast(msg);

    } else {
        delete msg; // unknown packet
    }
}


void PBFT::findFriendModules(){
    chainModule = check_and_cast<Blockchain*> (getParentModule()->getSubmodule("chain"));
    replicaStateModule = check_and_cast<ReplicaState*> (getParentModule()->getSubmodule("replicaState"));
}

void PBFT::initializeFriendModules(){
    chainModule->initializeChain();
    replicaStateModule->initializeState();

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

    PBFTMessage *myMsg = dynamic_cast<PBFTMessage*>(msg);
    numReceived ++;

    switch(myMsg->getType()){
        case REQUEST:{
            // Cast again the message to the needed format
            PBFTRequestMessage *req = dynamic_cast<PBFTRequestMessage*>(myMsg);
            EV << "sender key:" << req->getOp().getOriginatorKey() << endl;

            // Insert into log, save things like digest, sender, etc
            replicaStateModule->addToLog(req);

            // If I am the primary and if authentication is OK, assign sequence number and multicast PREPREPARE message
            if (replicaStateModule->getPrimary()) {
                sequenceNumber ++;
                PBFTPreprepareMessage* preprepare_msg = new PBFTPreprepareMessage("PBFTMessage");
                preprepare_msg->setView(replicaStateModule->getCurrentView());
                preprepare_msg->setSeqNumber(sequenceNumber);

                preprepare_msg->setDigest(req->getOp().computeHash());

                // TODO compute message digest
                broadcast(preprepare_msg);


/*
                Operation op = Operation(this->overlay->getThisNode().getKey(), thisNode.getIp(), simTime());
                msg->setOp(op);
                msg->setTimestamp(simTime());
                msg->setClientKey(this->overlay->getThisNode().getKey());
                msg->setClientAddress(thisNode); // This includes also the port, like 10.0.0.1:2048
*/
            }




            break;
        }

        case PREPREPARE:
            break;

        case PREPARE:
            break;

        case COMMIT:
            break;

        case REPLY: {
            PBFTReplyMessage *rep = dynamic_cast<PBFTReplyMessage*>(myMsg);
            break;
        }

        default:
            error("handleUDPMessage(): Unknown message type!");
            break;
    }

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

void PBFT::broadcast(cMessage* msg){
    if(DEBUG)
        EV << "[PBFT::broadcast() @ " << thisNode.getIp()
           << endl;

    PBFTMessage *myMsg = dynamic_cast<PBFTMessage*>(msg);

    // This call returns k random nodes from the partialView, + myself
    NodeVector* nodes = callNeighborSet(k);

    for(int i=0; i<(int)nodes->size(); i++){

        // send UDP message
        nodes->at(i).setPort(2048);
        if (nodes->at(i).getIp() == thisNode.getIp()){
            // Need to add some delay since I am sending the message to myself.
            sendMessageToUDP(nodes->at(i), myMsg->dup(), uniform(0, 1));
        } else {
            sendMessageToUDP(nodes->at(i), myMsg->dup());
        }

        numSent ++;
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

bool PBFT::isPrimary(){
    double k = this->overlay->getThisNode().getKey().toDouble();
    if (replicaStateModule->getCurrentView()%4 == k - 1){ // TODO Change the number of replicas (now 4).
        EV << "Replica IP: " << thisNode.getIp() << " IS PRIMARY" << endl;
        replicaStateModule->setPrimary(true);
        return true;
    }

    return false;
}







