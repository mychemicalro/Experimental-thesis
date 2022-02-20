
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

    blockCapacity = par("blockCapacity");


    // TODO watermarks advance mode
    h = 0;
    H = 128;

    S_PREPREPARE = false;
    S_PREPARE = false;
    S_COMMIT = false;

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

        case READY: {
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
            if (isPrimary()){
                nextBlock = new Block(blockCapacity);
            }

            // TODO For now add in all the replicas the initial blockchain block
            Block* initialBlock = new Block(blockCapacity);
            sequenceNumber ++;
            initialBlock->setSeqNumber(sequenceNumber);
            initialBlock->setViewNumber(replicaStateModule->getCurrentView());
            initialBlock->computeHash().c_str();

            Operation op = Operation(OverlayKey(42), IPvXAddress("1.1.1.1"), 0);
            initialBlock->addOperation(op);

            chainModule->addBlock(*initialBlock);

            break;
        }

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
        EV << "Operation hash: " << op.getHash() << endl;
        PBFTRequestMessage* msg = new PBFTRequestMessage("PBFTRequestMessage");
        msg->setOp(op);
        msg->setTimestamp(simTime()); // TODO not always needed
        msg->setClientKey(this->overlay->getThisNode().getKey());
        msg->setClientAddress(thisNode); // This includes also the port, like 10.0.0.1:2048

        broadcast(msg);

        scheduleAt(simTime() + 10, clientTimer);

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
        case REQUEST: {
            // Cast again the message to the needed format
            PBFTRequestMessage *req = dynamic_cast<PBFTRequestMessage*>(myMsg);
            EV << "Received message at: " << thisNode.getIp() << " with digest: " << req->getOp().getHash() << endl;

            // Check if already seen
            if (replicaStateModule->seenRequest(msg)){
                EV << "Request already seen - Request not processed -- returning " << endl;
                return;
            }

            // Insert into log, save things like digest, sender, etc
            replicaStateModule->addToRequestsLog(req);

            // Gossip the request
            broadcast(req);

            // If I am the primary and if authentication is OK, assign sequence number and multicast PREPREPARE message
            if (replicaStateModule->getPrimary()) {
                /*
                EV << "PRIMARY: I am processing a request and trying to send Preprepare messages in broadcast" << endl;
                sequenceNumber ++;
                PBFTPreprepareMessage* preprepare_msg = new PBFTPreprepareMessage("PBFTPreprepareMessage");
                preprepare_msg->setView(replicaStateModule->getCurrentView());
                preprepare_msg->setSeqNumber(sequenceNumber);

                preprepare_msg->setDigest(req->getOp().getHash().c_str());

                broadcast(preprepare_msg);

                // I could also add here the PREPREPARE message to my log -> this might be an error
                // replicaStateModule->addToPrepreparesLog(preprepare_msg);
*/

                // Blocks part
                nextBlock->addOperation(req->getOp());

                if (nextBlock->isFull()){
                    // get ready to broadcast the preprepare
                    sequenceNumber ++;
                    nextBlock->setSeqNumber(sequenceNumber);
                    nextBlock->setViewNumber(replicaStateModule->getCurrentView());

                    EV << "PRIMARY: Full block -> send PREPREPARE" << endl;

                    PBFTPreprepareMessage* preprepare_msg = new PBFTPreprepareMessage("PBFTPreprepareMessage");
                    preprepare_msg->setView(replicaStateModule->getCurrentView());
                    preprepare_msg->setSeqNumber(sequenceNumber);
                    preprepare_msg->setDigest(nextBlock->computeHash().c_str());
                    preprepare_msg->setBlock(*nextBlock);

                    broadcast(preprepare_msg);

                    vector<Operation> const & ops = nextBlock->getOperations();
                    for(size_t i=0; i<ops.size(); i++){
                        EV <<" chash " << ops.at(i).cHash() << endl;
                        EV << "gethash " << ops.at(i).getHash() << endl;
                    }
                }
            }

            break;
        }

        case PREPREPARE: {
            PBFTPreprepareMessage *req = dynamic_cast<PBFTPreprepareMessage*>(myMsg);
            /**
             * Once accepted:
             *      - enters the prepare phase
             *      - adds m in its log !? TODO -> also the original message comes with the PREPREPARE message
             *      - multicasts a PREPARE message
             */

            /**
             * I have to reason with blocks now.
             * A replica accepts this block if:
             *      - it has same view
             *      - message is authentic -> not controlled by us
             *      - n between h and H
             *      - the replica has not accepted a PRE-PREPARE block with
             *          same view and seqNumber, but with different digest.
             */

            // Check if already processed this message?
            if (replicaStateModule->seenRequest(req)){
                EV << "Preprepare already seen - Preprepare not processed -- returning " << endl;
                return;
            }

            // I have to gossip it -> Gossip the request in any case? (Like different view, wrong seqNum) TODO
            // If I am not the primary, I can broadcast it since the primary has already broadcasted the PREPREPARE

            if (!isPrimary()){
                broadcast(req);
            }

            if (replicaStateModule->getCurrentView() != req->getView()){
                EV << "Wrong view! - Preprepare not processed -- returning " << endl;
                return;
            }

            if (req->getSeqNumber() < h || req->getSeqNumber() > H){
                EV << "Wrong seqNumber! - Preprepare not processed -- returning " << endl;
                return;
            }
            // If I'have already accepted a PREPREPARE message for the same v and seqNum ... TODO
            if (S_PREPARE){
                EV << "Already in PREPARE phase! - Preprepare not processed -- returning " << endl;
                return;
            }

            // Now the replica is ready to accept the PREPREPARE message
            EV << "Replica accepted a PREPREPARE block! " << endl;
            replicaStateModule->addToPrepreparesLog(req);

            // TODO save somewhere the PREPREPARE message associated when entered the PREPARE phase


            // If the replica does not have the message in the log what happens? TODO
            // Need to check if I have in my log all the operations contained in the block.

            bool canPrepare = true;

            vector<Operation> const & ops = req->getBlock().getOperations();
            EV <<"Block view: " << req->getBlock().getCapacity() << endl;
            EV <<"Block hash: " << req->getBlock().getHash() << endl;
            for(size_t i=0; i<ops.size(); i++){
                EV << "Op hash: " << ops.at(i).cHash() << endl;
                if(!replicaStateModule->digestInRequestsLog(ops.at(i).cHash().c_str())){
                    canPrepare = false;
                }
            }

            if (canPrepare){
                // S_PREPARE = true;

                // Add the message to the seen ones
                replicaStateModule->addToPrepreparesLog(req);

                // multicast a PREPARE message
                EV << "Replica has to multicast a prepare message " << endl;

                PBFTPrepareMessage* prepare_msg = new PBFTPrepareMessage("PBFTPrepareMessage");
                prepare_msg->setView(replicaStateModule->getCurrentView());
                prepare_msg->setSeqNumber(req->getSeqNumber());
                prepare_msg->setDigest(req->getDigest());
                prepare_msg->setCreatorAddress(thisNode);
                prepare_msg->setCreatorKey(overlay->getThisNode().getKey());

                broadcast(prepare_msg);

                // Add the block to the candidateBlocks
                // candidateBlocks.push_back(req->getBlock());
                candidateBlocks.insert(make_pair(req->getBlock().getHash(), req->getBlock()));
                EV << "CandidateBlocks size: " << candidateBlocks.size() << endl;

            }

            break;
        }

        case PREPARE: {
            PBFTPrepareMessage *req = dynamic_cast<PBFTPrepareMessage*>(myMsg);


            if (replicaStateModule->seenRequest(req)){
                EV << "Prepare already seen - Prepare not processed -- returning " << endl;
                return;
            }

            EV << "Received new PREPARE message " << endl;
            replicaStateModule->addToPreparesLog(req);

            // Gossip the message If I see it for the first time, at any condition for now
            broadcast(req);


            if (replicaStateModule->searchPreparedCertificate(req)){
                // PreparedCertificate found! --> send COMMIT

                PBFTCommitMessage* commit_msg = new PBFTCommitMessage("PBFTCommitMessage");
                commit_msg->setView(req->getView());
                commit_msg->setSeqNumber(req->getSeqNumber());
                commit_msg->setDigest(req->getDigest());
                commit_msg->setCreatorAddress(thisNode);
                commit_msg->setCreatorKey(overlay->getThisNode().getKey());

                broadcast(commit_msg);
            }

            break;
        }

        case COMMIT: {
            PBFTCommitMessage *req = dynamic_cast<PBFTCommitMessage*>(myMsg);

            if (replicaStateModule->seenRequest(req)){
                EV << "Commit already seen - Commit not processed -- returning " << endl;
                return;
            }

            EV << "Received new COMMIT message " << endl;
            replicaStateModule->addToCommitsLog(req);

            // Gossip the message If I see it for the first time, at any condition for now
            broadcast(req);

            if(replicaStateModule->searchCommittedCertificate(req)){
                // Ready to commit
                // I need to know the block I have to add to the blockchain.
                EV << "Ready to commit" << endl;
                map<string, Block>::iterator it = candidateBlocks.find(req->getDigest());
                if (it != candidateBlocks.end()){
                    EV << "Print somewhat found" << endl;

                    // add the block if all the blocks with a lower seqNumber were added.
                    chainModule->addBlock(it->second);

                    // Send reply to client, How? as always, trying to broadcast the result ... or through an RPC call?
                    PBFTReplyMessage* reply_msg = new PBFTReplyMessage("PBFTReplyMessage");
                    reply_msg->setView(req->getView());

                    // Add the request timestamp .. Find it -> TODO
                    // Add the client key .. Find it -> TODO

                    reply_msg->setReplicaNumber(this->overlay->getThisNode().getKey());
                    reply_msg->setOperationResult(OK);

                    broadcast(reply_msg);
                }
            }

            break;
        }

        case REPLY: {
            PBFTReplyMessage *rep = dynamic_cast<PBFTReplyMessage*>(myMsg);
            EV << "Received a reply message from: " << rep->getReplicaNumber() << endl;

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




