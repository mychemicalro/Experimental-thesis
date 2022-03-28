
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

    h = 0;
    checkpointPeriod = par("checkpointPeriod"); // 16
    H = 4 * checkpointPeriod; // Log size

    WATCH(numSent);
    WATCH(numReceived);
    WATCH(k);
    WATCH(joinDelay);
    WATCH(requestDelay);
    WATCH(sequenceNumber);
    WATCH(H);
    WATCH(h);

    numRequests = 0;
    numPreprepares = 0;
    numPrepares = 0;
    numCommits = 0;
    numReplies = 0;
    numCheckpoints = 0;
    numUpdates = 0;
    numUpdateRequests = 0;


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


            break;

        case READY: {
            /**
             * The node is READY to work as a replica/replica+client.
             */
            state = READY;

            // Let's make this a client TODO This will have to be removed

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

            // Check if this is the primary, and set the state accordingly.
            if (isPrimary()){
                nextBlock = new Block(blockCapacity);
            }

            // If this is the very first node in the network
            if (this->overlay->getThisNode().getKey().toDouble() == 1){
                if(DEBUG)
                    EV << "This is the first node of the network" << endl;
                addFirstBlock();

            } else {
                // Create a fake actualRequest
                Operation op = Operation(OverlayKey(42), IPvXAddress("1.1.1.1"), 0);
                // This request is needed for initializing the actualRequest ..
                PBFTRequestMessage* msg = new PBFTRequestMessage("PBFTRequestMessage");
                msg->setOp(op);
                msg->setBitLength(PBFTREQUEST(msg));
                actualRequest = msg->dup();
                delete msg;

            }
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
    if (msg == clientTimer){
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

        if(actualRequest){
            delete actualRequest;
        }

        actualRequest = msg->dup();

        sendToMyNode(msg);
        delete msg;

        scheduleAt(simTime() + replyDelay, replyTimer);

    } else if(msg == replyTimer){
        // I know the timestamp of the request
        if(DEBUG)
            EV << "Broadcasting again request:"  << actualRequest->getOp().getHash() << endl;

        actualRequest->setRetryNumber(actualRequest->getRetryNumber() + 1);
        broadcast(actualRequest);

        // Record retry numbers
        if(nodeType == REPLICAANDCLIENT){
            globalStatistics->recordHistogram("PBFT: Retry numbers", actualRequest->getRetryNumber());
        }

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
        case REQUEST:
            handleRequestMessage(msg);
            break;

        case PREPREPARE:
            handlePreprepareMessage(msg);
            break;

        case PREPARE:
            handlePrepareMessage(msg);
            break;

        case COMMIT:
            handleCommitMessage(msg);
            break;

        case REPLY:
            handleReplyMessage(msg);
            break;

        case CHECKPOINT:
            handleCheckpointMessage(msg);
            break;

        case UPDATE_REQUEST:
            handleUpdateRequestMessage(msg);
            break;

        case UPDATE:
            handleUpdateMessage(msg);
            break;

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

    if(overlay->getState() == 6 && joined && chainModule->getBlockchainLength() == 0){
       if(DEBUG)
           EV << "Sending update request to my neighbors" << endl;

       // Send Update request message
       PBFTUpdateRequestMessage* updateRequest = new PBFTUpdateRequestMessage();

       updateRequest->setCreatorAddress(thisNode);
       updateRequest->setCreatorKey(overlay->getThisNode().getKey());
       updateRequest->setBitLength(PBFTUPDATEREQUEST(update));

       broadcast(updateRequest);
       delete updateRequest;

    }

    // TODO Understand what are the next steps eventually

}

void PBFT::broadcast(cMessage* msg){
    if(DEBUG)
        EV << "[PBFT::broadcast() @ " << thisNode.getIp()
           << endl;

    PBFTMessage *myMsg = dynamic_cast<PBFTMessage*>(msg);

    // This call returns k random nodes from the partialView, + myself
    NodeVector* nodes = callNeighborSet(k);

    if(DEBUG)
        EV << "Got neighbors for broadcast: " << nodes->size() << endl;

    for(int i=0; i<(int)nodes->size(); i++){

        nodes->at(i).setPort(2048);

        if (nodes->at(i).getIp() == thisNode.getIp()){
            // sendMessageToUDP(nodes->at(i), myMsg->dup(), uniform(0.50, 0.100));
            continue; // TODO For now ignore this
        } else {
            sendMessageToUDP(nodes->at(i), myMsg->dup());
        }
        numSent ++;
        updateMessageStats(myMsg);
    }
}

void PBFT::sendToMyNode(cMessage* msg){
    if(DEBUG)
        EV << "[PBFT::sendToMyNode() @ " << thisNode.getIp()
           << endl;

    PBFTMessage *myMsg = dynamic_cast<PBFTMessage*>(msg);

    updateMessageStats(myMsg);

    thisNode.setPort(2048);
    sendMessageToUDP(thisNode, myMsg->dup(), uniform(0.50, 0.100));

    numSent ++;

}


void PBFT::handleLowerMessage(cMessage* msg){
    if(DEBUG)
        EV << "[PBFT::handleLowerMessage() @ " << thisNode.getIp()
           << endl;

    if(msg->getKind() == KIND_READY){
        EV << "GOT READY TO GO MESSAGE" << endl;
        changeState(READY);
    }

    delete msg;
}

void PBFT::finishApp() {

    globalStatistics->addStdDev("PBFT: Sent packets", numSent);
    globalStatistics->addStdDev("PBFT: Received packets", numReceived);
    globalStatistics->addStdDev("PBFT: Sent request packets", numRequests);
    globalStatistics->addStdDev("PBFT: Sent preprepares packets", numPreprepares);
    globalStatistics->addStdDev("PBFT: Sent prepares packets", numPrepares);
    globalStatistics->addStdDev("PBFT: Sent commits packets", numCommits);
    globalStatistics->addStdDev("PBFT: Sent replies packets", numReplies);
    globalStatistics->addStdDev("PBFT: Sent checkpoints packets", numCheckpoints);
    globalStatistics->addStdDev("PBFT: Sent updates packets", numUpdates);

    if(nodeType == REPLICAANDCLIENT){
        EV << "Node is client: " << this->overlay->getThisNode().getKey() << endl;
        globalStatistics->recordHistogram("PBFT: Number of clients", 1);
    }
    replicaStateModule->clearDataStructures();

    if(nodeType == REPLICAANDCLIENT){
        delete actualRequest;
    }

}

bool PBFT::isPrimary(){
    double k = this->overlay->getThisNode().getKey().toDouble();
    if (replicaStateModule->getCurrentView() % replicaStateModule->getNodesNumber() == k - 1){ // TODO Change the number of replicas (now 4).
        if(DEBUG)
            EV << "Replica IP: " << thisNode.getIp() << " IS PRIMARY" << endl;

        replicaStateModule->setPrimary(true);
        return true;
    }

    return false;
}


void PBFT::handleRequestMessage(cMessage* msg){

    PBFTRequestMessage* req = dynamic_cast<PBFTRequestMessage*>(msg);

    if(DEBUG)
        EV << "Received PBFTRequest at: " << thisNode.getIp() << " with digest: " << req->getOp().getHash() << endl;

    size_t blockIndex = chainModule->isPresentOp(req->getOp());

    // Check if request already seen
    if (replicaStateModule->seenMessage(req)){
        if(DEBUG)
            EV << "Request: " << req->getOp().getHash() << " already seen -- retryNumber: " << req->getRetryNumber() << endl;
        if(blockIndex != -1){
            // broadcast reply (with the same increased retryNumber)

            Block bl = chainModule->getBlockByIndex(blockIndex);

            PBFTReplyMessage* reply_msg = new PBFTReplyMessage("PBFTReplyMessage");
            reply_msg->setView(replicaStateModule->getCurrentView());
            reply_msg->setReplicaNumber(this->overlay->getThisNode().getKey());
            reply_msg->setCreatorAddress(thisNode);
            reply_msg->setCreatorKey(overlay->getThisNode().getKey());
            reply_msg->setBitLength(PBFTREPLY(reply_msg));
            reply_msg->setRetryNumber(req->getRetryNumber());
            reply_msg->setBlock(bl);

            broadcast(reply_msg);

            if(DEBUG)
                EV << "Sending reply ... " << endl;

            delete reply_msg;

        }

        // delete req;
        return;
    }

    // TODO Discard requests with a lower timestamp than the last reply timestamp sent to this client -> exactly once semantics.

    // Insert into log, save things like digest, sender, etc
    replicaStateModule->addToRequestsLog(req);

    // Gossip the request
    broadcast(req);

    // Intermediate step!
    // This may be a retry from the client ...
    if(blockIndex != -1){

        Block bl = chainModule->getBlockByIndex(blockIndex);

        PBFTReplyMessage* reply_msg = new PBFTReplyMessage("PBFTReplyMessage");

        reply_msg->setView(replicaStateModule->getCurrentView());
        reply_msg->setReplicaNumber(this->overlay->getThisNode().getKey());
        reply_msg->setCreatorAddress(thisNode);
        reply_msg->setCreatorKey(overlay->getThisNode().getKey());
        reply_msg->setBitLength(PBFTREPLY(reply_msg));
        reply_msg->setRetryNumber(req->getRetryNumber());
        reply_msg->setBlock(bl);
        broadcast(reply_msg);

        if(DEBUG)
            EV << "Sending reply ... " << endl;

        delete reply_msg;

        // I need to end here
        return;
    }

    // If I am the primary and if authentication is OK, assign sequence number and multicast PREPREPARE message

    // If request already seen (but with different retry number), do not add the block if
    // the operations has been already included in some PREPREPARE.
    // So, I need to check that I have not a PREPREPARE message with a block containing this operation.

    if (replicaStateModule->getPrimary() && !replicaStateModule->operationPrepPrepared(req->getOp())) {

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

            // broadcast(preprepare_msg); // Why also broadcast?
            sendToMyNode(preprepare_msg);

            delete preprepare_msg;

            // clear nextBlock
            nextBlock = new Block(blockCapacity);
        }
    }

    onDemandPrePrepare(req); // TODO To call in any case ..? In case I accept this request.
}


void PBFT::handlePreprepareMessage(cMessage* msg){

    PBFTPreprepareMessage *preprep = dynamic_cast<PBFTPreprepareMessage*>(msg);

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
    if (replicaStateModule->seenMessage(preprep)){
        if(DEBUG)
            EV << "Preprepare already seen - Preprepare not processed -- returning " << endl;

        delete preprep;
        return;
    }

    // I have to gossip it -> Gossip the preprepare in any case? (Like different view, wrong seqNum) TODO
    // If I am not the primary, I can broadcast it since the primary has already broadcasted the PREPREPARE

    broadcast(preprep);

    if (replicaStateModule->getCurrentView() != preprep->getView()){
        if(DEBUG)
            EV << "Wrong view! - Preprepare not processed -- returning " << endl;
        return;
    }

    if (preprep->getSeqNumber() < h || preprep->getSeqNumber() > H){
        if(DEBUG)
            EV << "Wrong seqNumber! - Preprepare not processed -- returning " << endl;
        return;
    }

    // If I'have already accepted a PREPREPARE message for the same v and seqNum but with different digest
    if (replicaStateModule->otherPreprepareAccepted(preprep)){
        if(DEBUG)
            EV << "Already accepted PREPREPARE message for seqNum: " << preprep->getSeqNumber() << " and view: " << preprep->getView() << endl;
        return;
    }

    // Now the replica is ready to accept the PREPREPARE message
    if(DEBUG)
        EV << "Replica accepted a PREPREPARE block!" << endl;

    replicaStateModule->addToPrepreparesLog(preprep);

    replicaStateModule->addTimestamp(preprep);

    // If the replica does not have the message in the log what happens? TODO

    // Need to check if I have in my log all the operations contained in the block.
    bool canPrepare = true;

    vector<Operation> const & ops = preprep->getBlock().getOperations();
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
            EV << "Replica has to multicast a prepare message for block (inside handle): " << preprep->getBlock().getHash() << endl;

        PBFTPrepareMessage* prepare_msg = new PBFTPrepareMessage("PBFTPrepareMessage");
        prepare_msg->setView(replicaStateModule->getCurrentView());
        prepare_msg->setSeqNumber(preprep->getSeqNumber());
        prepare_msg->setDigest(preprep->getDigest());
        prepare_msg->setCreatorAddress(thisNode);
        prepare_msg->setCreatorKey(overlay->getThisNode().getKey());
        prepare_msg->setBitLength(PBFTPREPARE(prepare_msg));

        // broadcast(prepare_msg);
        sendToMyNode(prepare_msg);
        delete prepare_msg;

        // Add the block to the candidateBlocks
        replicaStateModule->addCandidateBlock(preprep);

    } else {
        if(DEBUG)
            EV << "Node: " << thisNode.getIp() << " cannot prepare message" << endl;
    }

}

void PBFT::handlePrepareMessage(cMessage* msg){
    PBFTPrepareMessage* prep = dynamic_cast<PBFTPrepareMessage*>(msg);

    if (replicaStateModule->seenMessage(prep)){
        if(DEBUG)
            EV << "Prepare already seen - Prepare not processed -- returning " << endl;
        // delete prep;
        return;
    }

    if(DEBUG)
        EV << "Received new PREPARE message " << endl;

    replicaStateModule->addToPreparesLog(prep);

    // Gossip the message since I see it for the first time, at any condition for now
    broadcast(prep);

    if (replicaStateModule->searchPreparedCertificate(prep)){

        // get block
        Block myBlock = replicaStateModule->getBlock(prep->getDigest());
        if(chainModule->isPresent(myBlock)){
            EV << "Block: " << myBlock.getHash() << " already present in this blockchain" << endl;
            return;
        }

        PBFTCommitMessage* commit_msg = new PBFTCommitMessage("PBFTCommitMessage");
        commit_msg->setView(prep->getView());
        commit_msg->setSeqNumber(prep->getSeqNumber());
        commit_msg->setDigest(prep->getDigest());
        commit_msg->setCreatorAddress(thisNode);
        commit_msg->setCreatorKey(overlay->getThisNode().getKey());
        commit_msg->setBitLength(PBFTCOMMIT(commit_msg));

        // broadcast(commit_msg);
        sendToMyNode(commit_msg);
        delete commit_msg;
    }
}


void PBFT::handleCommitMessage(cMessage* msg){
    PBFTCommitMessage* comm = dynamic_cast<PBFTCommitMessage*>(msg);
    if(DEBUG)
        EV << "Received new COMMIT message " << endl;

    if (replicaStateModule->seenMessage(comm)){
        if(DEBUG)
            EV << "Commit already seen - Commit not processed -- returning " << endl;
        // delete comm;
        return;
    }

    replicaStateModule->addToCommitsLog(comm);

    // Gossip the message If I see it for the first time, and the request is not mine
    broadcast(comm);

    if(replicaStateModule->searchCommittedCertificate(comm)){

        // get block
        Block myBlock = replicaStateModule->getBlock(comm->getDigest());

        if(chainModule->isPresent(myBlock)){
            EV << "Block: " << myBlock.getHash() << " already present in this blockchain" << endl;
            return;
        }

        bool canExecute = true;
        // TODO add the block if all the blocks with a lower seqNumber were added. How? Are we sure about this?

        map<string, Block> candidateBlocks = replicaStateModule->getCandidateBlocks();
        map<string, Block>::iterator cb;

        for(cb=candidateBlocks.begin(); cb!=candidateBlocks.end(); cb++){
            if (cb->second.getSeqNumber() < myBlock.getSeqNumber()){
                if(!chainModule->isPresent(cb->second)){
                    if(DEBUG)
                        EV << "Have to wait before executing this block ..." << endl;
                        EV << "The previous block: " << cb->second.getHash() << " with seqnum: " << cb->second.getSeqNumber() << " was not added to the blockchain" << endl;

                    canExecute = false;
                    break;
                }
            }
        }

        if(canExecute){
            // Ready to commit/execute
            chainModule->addBlock(myBlock);

            double insertionTimestamp = replicaStateModule->getTimestamp(myBlock.getHash());
            double insertionTimestamp2 = myBlock.getCreationTimestamp();
            globalStatistics->addStdDev("PBFT: Blocks latency (when received)", (simTime().dbl() - insertionTimestamp));
            globalStatistics->addStdDev("PBFT: Blocks latency (when created)", (simTime().dbl() - insertionTimestamp2));

            if(comm->getSeqNumber() % checkpointPeriod == 0){
                createCheckpoint(comm->getSeqNumber());
            }

            PBFTReplyMessage* reply_msg = new PBFTReplyMessage("PBFTReplyMessage");
            // vector<Operation> const & ops = myBlock.getOperations();
            reply_msg->setReplicaNumber(this->overlay->getThisNode().getKey());
            reply_msg->setCreatorAddress(thisNode);
            reply_msg->setCreatorKey(overlay->getThisNode().getKey());
            reply_msg->setBitLength(PBFTREPLY(reply_msg));
            reply_msg->setBlock(myBlock);
            reply_msg->setView(comm->getView());

            sendToMyNode(reply_msg);
            delete reply_msg;

            /**
             * This block may halt some other blocks that already had a committed certificate, but were not executed
             * since have a greater seqnumber.
             *
             */
            onDemandCommit(comm->getSeqNumber());
        }
    }
}


void PBFT::handleReplyMessage(cMessage* msg){
    PBFTReplyMessage* rep = dynamic_cast<PBFTReplyMessage*>(msg);

    /**
     * Check if I have already seen the message -> stop
     * Add the message to the log and broadcast it.
     * If I am the client, check for the weak certificate (reply certificate).
     * If found, accept the reply.
     */

    if(replicaStateModule->seenMessage(rep)){
        if(DEBUG)
            EV << "Reply already seen - Reply not processed -- returning " << endl;
        // delete rep;
        return;
    }
    // I do not have the hash here!
    EV << "BLOCK HASH IN HANDLEREPLY: " << rep->getBlock().getHash()
        << " capacity: " << rep->getBlock().getCapacity()
        << " from: " << rep->getCreatorKey()
        << " at: " << this->overlay->getThisNode().getKey()
        << endl;

    replicaStateModule->addToRepliesLog(rep);

    Operation op;
    int err = 1;
    try {
        op = actualRequest->getOp();
        err = op.getTimestamp().dbl();
    } catch (std::exception& e){
        EV << "Exception caught : " << e.what() << endl;
        err = -1;
    }

    if (err != 0 && nodeType == REPLICAANDCLIENT){

        if(rep->getBlock().containsOp(op) /* || rep->getOp().getOriginatorKey() == this->overlay->getThisNode().getKey()*/ ){
            if(DEBUG){
                EV << "Received a reply message from: " << rep->getReplicaNumber()
                   << " For the request: " << actualRequest->getOp().getHash()
                   << endl;

                EV << "Received reply with block sn: " << rep->getBlock().getSeqNumber()
                   << " hash: " << rep->getBlock().getHash()
                   << " capacity: " << rep->getBlock().getCapacity()
                   << endl;

            }
            // The client waits for a weak certificate of f+1 replies. This will be called reply certificate.
            if(replicaStateModule->searchReplyCertificate(rep)){
                if(DEBUG){
                    EV << "Found reply certificate for request: " << actualRequest->getOp().getHash() << endl;
                    EV << "Reply accepted" << endl;
                }

                globalStatistics->addStdDev("PBFT: Requests latency", (simTime().dbl() - actualRequest->getOp().getTimestamp()).dbl());

                // Delete the replyTimer ...
                cancelEvent(replyTimer);

                // TODO Try to not delete because of error! -> no error ...
                // delete actualRequest;

                // Create a new request immediately
                // scheduleAt(simTime() + requestDelay, clientTimer);
                scheduleAt(simTime(), clientTimer);
                return;
            }
        } else {
            if (DEBUG)
                EV << "Block: " << rep->getBlock().getHash() << " does not contain my request: " << actualRequest->getOp().getHash() << endl;
        }

    } else {
        broadcast(rep);
    }
}


void PBFT::handleCheckpointMessage(cMessage* msg){
    if(DEBUG)
        EV << "[PBFT::handleCheckpointMessage() @ " << thisNode.getIp()
           << endl;

    PBFTCheckpointMessage* chkp = dynamic_cast<PBFTCheckpointMessage*>(msg);

    if(replicaStateModule->seenMessage(chkp)){
        if(DEBUG)
            EV << "Checkpoint already seen - Checkpoint not processed -- returning " << endl;
        // delete chkp;
        return;
    }

    replicaStateModule->addToCheckpointsLog(chkp);
    broadcast(chkp);

    replicaStateModule->checkpointProcedure(chkp->getSeqNumber());
}

void PBFT::handleUpdateRequestMessage(cMessage* msg){
    if(DEBUG)
        EV << "[PBFT::handleUpdateRequestMessage() @ " << thisNode.getIp()
           << endl;

    if (state != READY){
        if(DEBUG)
            EV << "Node not ready for sending update messages to new neighbor" << endl;
        return;
    }

    PBFTUpdateRequestMessage* updateRequest = dynamic_cast<PBFTUpdateRequestMessage*>(msg);

    PBFTUpdateMessage* update = new PBFTUpdateMessage();
    update->setSeqNumber(sequenceNumber);
    update->setView(replicaStateModule->getCurrentView());
    update->setLowH(h);
    update->setHighH(H);
    update->setBlockchain_length(chainModule->getBlockchainLength());
    update->setCreatorAddress(thisNode);
    update->setCreatorKey(overlay->getThisNode().getKey());
    update->setBitLength(PBFTUPDATE(update));
    update->setBlockchainModuleId(chainModule->getBlockchainId());

    TransportAddress addr = updateRequest->getCreatorAddress();
    addr.setPort(2048);
    sendMessageToUDP(addr, update->dup());
    updateMessageStats(update);
    numSent ++;
    delete update;

}

void PBFT::handleUpdateMessage(cMessage* msg){
    if(DEBUG)
        EV << "[PBFT::handleUpdateMessage() @ " << thisNode.getIp()
           << endl;

    PBFTUpdateMessage* update = dynamic_cast<PBFTUpdateMessage*>(msg);

    if (state != READY){
        if(DEBUG)
            EV << "Node still not ready - returning" << endl;
        return;
    }


    // Try to update my state
    if (update->getBlockchain_length() > chainModule->getBlockchainLength()){
        h = update->getLowH();
        H = update->getHighH();
        replicaStateModule->setCurrentView(update->getView());
        sequenceNumber = update->getSeqNumber();
        EV << "More recent data blockchain length: " << update->getBlockchain_length() << endl;
        EV << "Actual state: " << state << endl;

        // Update blockchain -> schifezza
        cModule* module = simulation.getModule(update->getBlockchainModuleId());

        if(module && dynamic_cast<Blockchain*>(module)) {
            Blockchain* v = check_and_cast<Blockchain*>(module);
            EV << "Other blockchain length: " << v->getBlockchainLength() << endl;
            vector<Block> blocchetti = v->getBlocks();
            EV << "Other blockchain length blocchetti: " << blocchetti.size() << endl;

            chainModule->updateBlockchain(blocchetti);
        }
    }
}


void PBFT::onDemandPrePrepare(PBFTRequestMessage* req){
    if(DEBUG)
        EV << "[PBFT::onDemandPrePrepare() @ " << thisNode.getIp()
           << endl;

    if(!replicaStateModule->checkIfCanPrepare(req)){
        return;
    }

    PBFTPreprepareMessage preprep = replicaStateModule->getPreprepareMessage(req->getOp());

    PBFTPrepareMessage* prepare_msg = new PBFTPrepareMessage("PBFTPrepareMessage");
    prepare_msg->setView(replicaStateModule->getCurrentView());
    prepare_msg->setSeqNumber(preprep.getSeqNumber());
    prepare_msg->setDigest(preprep.getDigest());
    prepare_msg->setCreatorAddress(thisNode);
    prepare_msg->setCreatorKey(overlay->getThisNode().getKey());
    prepare_msg->setBitLength(PBFTPREPARE(prepare_msg));

    sendToMyNode(prepare_msg);
    delete prepare_msg;

    replicaStateModule->addCandidateBlock(&preprep);

    if(DEBUG)
        EV << "Replica has to multicast a prepare message for block: " << preprep.getBlock().getHash() << endl;

}


void PBFT::onDemandCommit(int sn){
    if(DEBUG)
        EV << "[PBFT::onDemandCommit() @ " << thisNode.getIp()
           << endl;

    // get all commit messages with seqnum > sn
    vector<PBFTCommitMessage> comms = replicaStateModule->getCommitMessages(sn);

    for(size_t j=0; j<comms.size(); j++){

        if(replicaStateModule->searchCommittedCertificate(&comms.at(j))){

            // get block
            Block myBlock = replicaStateModule->getBlock(comms.at(j).getDigest());

            if(chainModule->isPresent(myBlock)){
                if(DEBUG)
                    EV << "Block: " << myBlock.getHash() << " already present in this blockchain" << endl;
                // return; // -> wrong
                continue;
            }

            bool canExecute = true;

            map<string, Block> candidateBlocks = replicaStateModule->getCandidateBlocks();
            map<string, Block>::iterator cb;

            for(cb=candidateBlocks.begin(); cb!=candidateBlocks.end(); cb++){
                if (cb->second.getSeqNumber() < myBlock.getSeqNumber()){
                    if(!chainModule->isPresent(cb->second)){
                        if(DEBUG){
                            EV << "Have to wait before executing this block ..." << endl;
                            EV << "The previous block: " << cb->second.getHash() << " with seqnum: " << cb->second.getSeqNumber() << " was not added to the blockchain" << endl;
                        }
                        canExecute = false;
                        break;
                    }
                }
            }

            if(canExecute){
                // Ready to commit/execute
                chainModule->addBlock(myBlock);

                double insertionTimestamp = replicaStateModule->getTimestamp(myBlock.getHash());
                double insertionTimestamp2 = myBlock.getCreationTimestamp();
                globalStatistics->addStdDev("PBFT: Blocks latency (when received)", (simTime().dbl() - insertionTimestamp));
                globalStatistics->addStdDev("PBFT: Blocks latency (when created)", (simTime().dbl() - insertionTimestamp2));

                if(comms.at(j).getSeqNumber() % checkpointPeriod == 0){
                    createCheckpoint(comms.at(j).getSeqNumber());
                }

                PBFTReplyMessage* reply_msg = new PBFTReplyMessage("PBFTReplyMessage");
                reply_msg->setReplicaNumber(this->overlay->getThisNode().getKey());
                reply_msg->setCreatorAddress(thisNode);
                reply_msg->setCreatorKey(overlay->getThisNode().getKey());
                reply_msg->setBitLength(PBFTREPLY(reply_msg));
                reply_msg->setBlock(myBlock);
                reply_msg->setView(comms.at(j).getView());

                sendToMyNode(reply_msg);
                delete reply_msg;
            }
        }
    }
}

void PBFT::createCheckpoint(int sn){
    if(DEBUG)
        EV << "[PBFT::createCheckpoint() @ " << thisNode.getIp()
           << " Sequence number is: " << sn
           << " Low watermark number is: " << h
           << " High watermark number is: " << H
           << endl;

    h = sn;
    H += 4*checkpointPeriod;

    PBFTCheckpointMessage* chkp_msg = new PBFTCheckpointMessage("PBFTCheckpointMessage");
    chkp_msg->setSeqNumber(sn);
    chkp_msg->setCreatorAddress(thisNode);
    chkp_msg->setCreatorKey(overlay->getThisNode().getKey());
    chkp_msg->setBitLength(PBFTCHECKPOINT(reply_msg));
    chkp_msg->setDigest(chainModule->getLastBlockHash().c_str());

    sendToMyNode(chkp_msg);
    delete chkp_msg;

}

void PBFT::addFirstBlock(){

    Operation op = Operation(OverlayKey(42), IPvXAddress("1.1.1.1"), 0);
    Block* initialBlock = new Block(blockCapacity);
    initialBlock->setPrevBlockHash("Big Bang");
    initialBlock->addOperation(op);
    initialBlock->setSeqNumber(sequenceNumber);
    initialBlock->setViewNumber(replicaStateModule->getCurrentView());


    chainModule->addBlock(*initialBlock);
    sequenceNumber ++;

    // This request is needed for initializing the actualRequest ..
    PBFTRequestMessage* msg = new PBFTRequestMessage("PBFTRequestMessage");
    msg->setOp(op);
    msg->setBitLength(PBFTREQUEST(msg));
    actualRequest = msg->dup();
    if(DEBUG)
        EV << "Initial request cloned: " << actualRequest->getOp().getHash() << endl;
    delete msg;

}

void PBFT::updateMessageStats(PBFTMessage* m){
    if(DEBUG)
        EV << "[PBFT::updateMessageStats() @ " << thisNode.getIp()
           << endl;

    if (m->getType() == REQUEST){
        numRequests ++;
    } else if (m->getType() == PREPREPARE){
        numPreprepares ++;
    } else if (m->getType() == PREPARE){
        numPrepares ++;
    } else if (m->getType() == COMMIT){
        numCommits ++;
    } else if (m->getType() == REPLY){
        numReplies ++;
    } else if (m->getType() == CHECKPOINT){
        numCheckpoints ++;
    } else if (m->getType() == UPDATE) {
        numUpdates ++;
    } else if (m->getType() == UPDATE_REQUEST) {
        numUpdateRequests ++;
    }

}

