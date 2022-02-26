
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
    clientProb = par("clientProb");

    numSent = 0;
    numReceived = 0;

    sequenceNumber = 0;

    blockCapacity = par("blockCapacity");

    // TODO watermarks advance mode
    h = 0;
    H = 256; // Log size
    checkpointPeriod = 128; // in this way, when a replica procesess block with seqNum = 128,
    // it will create a checkpoint, h will be 128 and H will be 256+128.

    WATCH(numSent);
    WATCH(numReceived);
    WATCH(k);
    WATCH(joinDelay);
    WATCH(requestDelay);
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
            // Decide if the node must be a client -> probability

            double prob = uniform(0, 1);

            if (prob < clientProb){
                nodeType = REPLICAANDCLIENT;
                if(DEBUG)
                    EV << "Replica: " << thisNode.getIp() << " is CLIENT" << endl;

                // Set timer for sending messages
                clientTimer = new cMessage("Client timer");
                replyTimer = new cMessage("Reply timer");

                replyDelay = par("replyDelay");
                requestDelay = par("requestDelay");

                scheduleAt(simTime() + requestDelay, clientTimer);
            }

            // TODO solve cancelAndDelete(joinTimer) issue in the destructor of PBFT

            // Check if this is the primary, and set the state accordingly.
            if (isPrimary()){
                nextBlock = new Block(blockCapacity);
            }

            // For now add in all the replicas the initial blockchain block, the same for all!
            /**
             * Initial block:
             *      - sequence number = 1
             *      - view = 0
             *      - key = 42
             *      - ip = 1.1.1.1
             *      - timestamp = 0
             */
            Block* initialBlock = new Block(blockCapacity);
            sequenceNumber ++;
            initialBlock->setSeqNumber(sequenceNumber);
            initialBlock->setViewNumber(replicaStateModule->getCurrentView());
            initialBlock->computeHash().c_str();
            initialBlock->setPrevBlockHash("Big Bang");

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

        // Start when the underlayConfigurator is ready
        if(underlayConfigurator->isInInitPhase()){
            scheduleAt(simTime() + requestDelay, clientTimer);
            return;
        }

        Operation op = Operation(this->overlay->getThisNode().getKey(), thisNode.getIp(), simTime());

        if(DEBUG)
            EV << "Client sending operation: " << op.getHash() << " at: " << simTime() << endl;

        PBFTRequestMessage* msg = new PBFTRequestMessage("PBFTRequestMessage");
        msg->setOp(op);
        msg->setBitLength(PBFTREQUEST(msg));

        // Remember the request for the replyTimer
        actualRequest = msg->dup();
        // broadcast(msg);
        sendToMyNode(msg); // just send to my node, it will broadcast the message for me
        delete msg;

        scheduleAt(simTime() + replyDelay, replyTimer);

    } else if(msg == replyTimer){
        // I know the timestamp of the request
        if(DEBUG)
            EV << "Broadcasting again request:"  << actualRequest->getOp().getHash() << endl;

        actualRequest->setRetryNumber(actualRequest->getRetryNumber() + 1);
        broadcast(actualRequest);

        scheduleAt(simTime() + replyDelay, replyTimer);

    } else {
        delete msg; // unknown packet
    }
}


void PBFT::findFriendModules(){
    chainModule = check_and_cast<Blockchain*> (getParentModule()->getSubmodule("chain"));
    replicaStateModule = check_and_cast<ReplicaState*> (getParentModule()->getSubmodule("replicaState"));
}

void PBFT::initializeFriendModules(){
    chainModule->initializeChain(&this->overlay->getThisNode().getKey());
    replicaStateModule->initializeState(&this->overlay->getThisNode().getKey());
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
            PBFTRequestMessage* req = dynamic_cast<PBFTRequestMessage*>(myMsg);
            if(DEBUG)
                EV << "Received PBFTRequest at: " << thisNode.getIp() << " with digest: " << req->getOp().getHash() << endl;

            // Check if request already seen
            if (replicaStateModule->seenMessage(msg)){
                if(DEBUG)
                    EV << "Request already seen -- retryNumber: " << req->getRetryNumber() << endl;

                // TODO Check if the request has already been processed. If yes, it should retransmit the reply.
                // What about the gossiping?
                // Replicas must remember the last reply message they sent to each client.

                if(replicaStateModule->requestHasReply(req)){
                    // broadcast reply (with an increased retryNumber)

                    PBFTReplyMessage* reply_msg = new PBFTReplyMessage("PBFTReplyMessage");
                    reply_msg->setView(replicaStateModule->getCurrentView());

                    reply_msg->setOp(req->getOp());
                    reply_msg->setReplicaNumber(this->overlay->getThisNode().getKey());
                    reply_msg->setOperationResult(OK);

                    reply_msg->setCreatorAddress(thisNode);
                    reply_msg->setCreatorKey(overlay->getThisNode().getKey());
                    reply_msg->setBitLength(PBFTREPLY(reply_msg));
                    reply_msg->setRetryNumber(req->getRetryNumber());

                    broadcast(reply_msg);

                    if(DEBUG)
                        EV << "Sending reply ... " << endl;

                    delete reply_msg;

                }

                delete req;
                return;
            }

            // TODO Discard requests with a lower timestamp than the last reply timestamp sent to this client -> exactly once semantics.

            // Insert into log, save things like digest, sender, etc
            replicaStateModule->addToRequestsLog(req);

            // Gossip the request
            broadcast(req);

            // Intermediate step!
            // This may be a retry from the client ...
            if(replicaStateModule->requestHasReply(req)){ // TODO How can I have a reply for a message I never saw?

                PBFTReplyMessage* reply_msg = new PBFTReplyMessage("PBFTReplyMessage");
                reply_msg->setView(replicaStateModule->getCurrentView());

                reply_msg->setOp(req->getOp());
                reply_msg->setReplicaNumber(this->overlay->getThisNode().getKey());
                reply_msg->setOperationResult(OK);

                reply_msg->setCreatorAddress(thisNode);
                reply_msg->setCreatorKey(overlay->getThisNode().getKey());
                reply_msg->setBitLength(PBFTREPLY(reply_msg));
                reply_msg->setRetryNumber(req->getRetryNumber());

                broadcast(reply_msg);
                if(DEBUG)
                    EV << "Sending reply ... " << endl;

                delete reply_msg;

                // I need to end here
                return;
            }

            // If I am the primary and if authentication is OK, assign sequence number and multicast PREPREPARE message
            if (replicaStateModule->getPrimary()) {

                nextBlock->addOperation(req->getOp());

                if (nextBlock->isFull()){
                    if(DEBUG)
                        EV << "PRIMARY: Full block -> send PREPREPARE" << endl;

                    // get ready to broadcast the preprepare
                    sequenceNumber ++;
                    nextBlock->setSeqNumber(sequenceNumber);
                    nextBlock->setViewNumber(replicaStateModule->getCurrentView());
                    nextBlock->setPrevBlockHash(chainModule->getLastBlockHash());

                    PBFTPreprepareMessage* preprepare_msg = new PBFTPreprepareMessage("PBFTPreprepareMessage");
                    preprepare_msg->setView(replicaStateModule->getCurrentView());
                    preprepare_msg->setSeqNumber(sequenceNumber);
                    preprepare_msg->setDigest(nextBlock->computeHash().c_str());
                    preprepare_msg->setBlock(*nextBlock);

                    preprepare_msg->setBitLength(PBFTPREPREPARE(preprepare_msg));
                    broadcast(preprepare_msg);
                    sendToMyNode(preprepare_msg);

                    delete preprepare_msg;

                    // clear nextBlock
                    nextBlock = new Block(blockCapacity);
                }
            }

            break;
        }

        case PREPREPARE: {
            PBFTPreprepareMessage *req = dynamic_cast<PBFTPreprepareMessage*>(myMsg);

            /**
             * I have to reason with blocks now.
             * A replica accepts this block if:
             *      - it has same view
             *      - message is authentic -> not controlled by us
             *      - n between h and H
             *      - the replica has not accepted a PRE-PREPARE block with
             *          same view and seqNumber, but with different digest.
             *
             * Once accepted:
             *      - enters the prepare phase
             *      - adds m in its log !? TODO -> also the original message comes with the PREPREPARE message
             *      - multicasts a PREPARE message
             */

            // Check if already processed this message?
            if (replicaStateModule->seenMessage(req)){
                if(DEBUG)
                    EV << "Preprepare already seen - Preprepare not processed -- returning " << endl;

                delete req;
                return;
            }

            // I have to gossip it -> Gossip the request in any case? (Like different view, wrong seqNum) TODO
            // If I am not the primary, I can broadcast it since the primary has already broadcasted the PREPREPARE

            /*
            if (!isPrimary()){
                broadcast(req);
            }
            */
            broadcast(req);

            if (replicaStateModule->getCurrentView() != req->getView()){
                if(DEBUG)
                    EV << "Wrong view! - Preprepare not processed -- returning " << endl;
                return;
            }

            if (req->getSeqNumber() < h || req->getSeqNumber() > H){
                if(DEBUG)
                    EV << "Wrong seqNumber! - Preprepare not processed -- returning " << endl;
                return;
            }

            // If I'have already accepted a PREPREPARE message for the same v and seqNum but with different digest
            if (replicaStateModule->otherPreprepareAccepted(req)){
                if(DEBUG)
                    EV << "Already accepted PREPREPARE message for seqNum: " << req->getSeqNumber() << " and view: " << req->getView() << endl;
                return;
            }

            // Now the replica is ready to accept the PREPREPARE message
            if(DEBUG)
                EV << "Replica accepted a PREPREPARE block! " << endl;
            replicaStateModule->addToPrepreparesLog(req);

            // If the replica does not have the message in the log what happens? TODO

            // Need to check if I have in my log all the operations contained in the block.
            bool canPrepare = true;

            vector<Operation> const & ops = req->getBlock().getOperations();
            for(size_t i=0; i<ops.size(); i++){
                if(!replicaStateModule->digestInRequestsLog(ops.at(i).cHash().c_str())){
                    canPrepare = false;
                    if(DEBUG)
                        EV << "Operation: " << ops.at(i).cHash().c_str() << " not received by this node" << endl;
                }
            }

            if (canPrepare){

                // multicast a PREPARE message
                if(DEBUG)
                    EV << "Replica has to multicast a prepare message " << endl;

                PBFTPrepareMessage* prepare_msg = new PBFTPrepareMessage("PBFTPrepareMessage");
                prepare_msg->setView(replicaStateModule->getCurrentView());
                prepare_msg->setSeqNumber(req->getSeqNumber());
                prepare_msg->setDigest(req->getDigest());
                prepare_msg->setCreatorAddress(thisNode);
                prepare_msg->setCreatorKey(overlay->getThisNode().getKey());
                prepare_msg->setBitLength(PBFTPREPARE(prepare_msg));

                // broadcast(prepare_msg);
                sendToMyNode(prepare_msg);
                delete prepare_msg;

                // Add the block to the candidateBlocks
                candidateBlocks.insert(make_pair(req->getBlock().getHash(), req->getBlock()));
            } else {
                if(DEBUG)
                    EV << "Node: " << thisNode.getIp() << " cannot prepare message" << endl;
            }

            break;
        }

        case PREPARE: {
            PBFTPrepareMessage *req = dynamic_cast<PBFTPrepareMessage*>(myMsg);
            if(DEBUG)
                EV << "Received new PREPARE message " << endl;

            if (replicaStateModule->seenMessage(req)){
                if(DEBUG)
                    EV << "Prepare already seen - Prepare not processed -- returning " << endl;
                delete req;
                return;
            }

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
                commit_msg->setBitLength(PBFTCOMMIT(commit_msg));

                // broadcast(commit_msg);
                sendToMyNode(commit_msg);

                delete commit_msg;
            }

            break;
        }

        case COMMIT: {
            PBFTCommitMessage *req = dynamic_cast<PBFTCommitMessage*>(myMsg);
            if(DEBUG)
                EV << "Received new COMMIT message " << endl;
            if (replicaStateModule->seenMessage(req)){
                if(DEBUG)
                    EV << "Commit already seen - Commit not processed -- returning " << endl;
                delete req;
                return;
            }

            replicaStateModule->addToCommitsLog(req);

            // Gossip the message If I see it for the first time, and the request is not mine
            broadcast(req);

            if(replicaStateModule->searchCommittedCertificate(req)){

                // I need to know the block I have to add to the blockchain.
                EV << "Ready to commit" << endl;

                map<string, Block>::iterator it = candidateBlocks.find(req->getDigest());
                if (it != candidateBlocks.end()){
                    if(DEBUG)
                        EV << "Candidate block found" << endl;

                    bool canExecute = true;
                    // TODO add the block if all the blocks with a lower seqNumber were added. How? Are we sure about this?
                    map<string, Block>::iterator cb;
                    for(cb=candidateBlocks.begin(); cb!=candidateBlocks.end(); cb++){
                        if (cb->second.getSeqNumber() < it->second.getSeqNumber()){
                            if(!chainModule->isPresent(cb->second)){
                                if(DEBUG)
                                    EV << "Have to wait before executing this block ..." << endl;
                                    EV << "The previous block: " << cb->second.getHash() << " with seqnum: " << cb->second.getSeqNumber() << " was not added to the blockchain" << endl;

                                canExecute = false;
                            }
                        }
                    }

                    if(canExecute){
                        // Ready to commit/execute
                        chainModule->addBlock(it->second);

                        /**
                         * Attention please.
                         * Since I have a block I have to reply to all the clients contained in the
                         * block's operations.
                         * For each operation in the block, get the operation and send the REPLY
                         *
                         */

                        vector<Operation> const & ops = it->second.getOperations();
                        for(size_t i=0; i<ops.size(); i++){

                            // Send reply to client, How? as always, trying to broadcast the result ... or through an RPC call?
                            PBFTReplyMessage* reply_msg = new PBFTReplyMessage("PBFTReplyMessage");
                            reply_msg->setView(req->getView());

                            reply_msg->setOp(ops.at(i));
                            reply_msg->setReplicaNumber(this->overlay->getThisNode().getKey());
                            reply_msg->setOperationResult(OK);

                            reply_msg->setCreatorAddress(thisNode);
                            reply_msg->setCreatorKey(overlay->getThisNode().getKey());
                            reply_msg->setBitLength(PBFTREPLY(reply_msg));

                            // TODO Remember this reply for the client, in case it's timer expires -> I have the replies vector.
                            // Can check there if I already have replies to some request.

                            broadcast(reply_msg);
                            sendToMyNode(reply_msg);

                            delete reply_msg;
                        }
                    }
                } else {
                    EV << "Candidate block not found! " << endl;

                }
            }

            break;
        }

        case REPLY: {
            PBFTReplyMessage *rep = dynamic_cast<PBFTReplyMessage*>(myMsg);

            /**
             * Check if I have already seen the message -> stop
             * Add the message to the log and broadcast it.
             * If I am the client, check for the weak certificate (reply certificate).
             * If found, accept the reply.
             */

            if(replicaStateModule->seenMessage(rep)){
                if(DEBUG)
                    EV << "Reply already seen - Reply not processed -- returning " << endl;
                delete rep;
                return;
            }

            replicaStateModule->addToRepliesLog(rep);
            broadcast(rep);

            if(rep->getOp().getOriginatorKey() == this->overlay->getThisNode().getKey()){
                if(DEBUG)
                    EV << "Received a reply message from: " << rep->getReplicaNumber() << endl;

                // The client waits for a weak certificate of f+1 replies. This will be called reply certificate.
                if(replicaStateModule->searchReplyCertificate(rep)){
                    EV << "Found reply certificate --> Accepted result: " << rep->getOperationResult() << endl;

                    if(DEBUG)
                        EV << "Request timestamp: " << rep->getOp().getTimestamp() << endl;
                        EV << "Actual simtime: " << simTime() << endl;

                    // Delete the replyTimer ...
                    cancelEvent(replyTimer);

                    // Create a new request
                    scheduleAt(simTime() + requestDelay, clientTimer);

                }
            }

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

        nodes->at(i).setPort(2048);

        if (nodes->at(i).getIp() == thisNode.getIp()){
            // sendMessageToUDP(nodes->at(i), myMsg->dup(), uniform(0.50, 0.100));
            continue; // TODO For now ignore this
        } else {
            sendMessageToUDP(nodes->at(i), myMsg->dup());
        }
        numSent ++;
    }
}

void PBFT::sendToMyNode(cMessage* msg){
    if(DEBUG)
        EV << "[PBFT::sendToMyNode() @ " << thisNode.getIp()
           << endl;

    PBFTMessage *myMsg = dynamic_cast<PBFTMessage*>(msg);
    thisNode.setPort(2048);
    sendMessageToUDP(thisNode, myMsg->dup(), uniform(0.50, 0.100));

    numSent ++;

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
        if(DEBUG)
            EV << "Replica IP: " << thisNode.getIp() << " IS PRIMARY" << endl;

        replicaStateModule->setPrimary(true);
        return true;
    }

    return false;
}


