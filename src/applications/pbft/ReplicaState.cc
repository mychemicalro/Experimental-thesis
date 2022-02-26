/*
 * ReplicaState.cc
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */


#define DEBUG true
#include "ReplicaState.h"

using namespace std;

Define_Module(ReplicaState);

void ReplicaState::initialize(int stage) {
    // because of IPAddressResolver, we need to wait until interfaces
    // are registered, address auto-assignment takes place etc.
    if(stage != MIN_STAGE_OVERLAY)
        return;
}

void ReplicaState::handleMessage(cMessage* msg){
    error("This module doesn't handle messages, it runs only in initialize()");
}

void ReplicaState::initializeState(const OverlayKey* ok) {


    currentView = 0;
    primary = false;
    requests.clear();
    preprepares.clear();
    prepares.clear();
    f = par("f");

    this->overlayk = ok;

    WATCH(primary);
    WATCH(currentView);
    WATCH(f);

    if(DEBUG){
        EV << "ReplicaState initialized" << endl;
        EV << "Replica with overlay key: " << *overlayk << endl;
    }

    // TODO
    // WATCH_VECTOR(requests);
    // WATCH_VECTOR(preprepares);
    // WATCH_VECTOR(prepares);

}

void ReplicaState::addToRequestsLog(PBFTRequestMessage* msg){

    if (seenMessage(msg)) return;
    requests.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTRequestMessage" << endl;
}

bool ReplicaState::digestInRequestsLog(const char* digest){

    for(size_t i=0; i<requests.size(); i++){
        EV << "request hash: " << requests.at(i).getOp().getHash().c_str() << " digest hash: " << digest << endl;
        if (strcmp(requests.at(i).getOp().getHash().c_str(), digest) == 0){
            if(DEBUG)
                EV << "Digest found - returning true" << endl;
            return true;
        }
    }

    if(DEBUG)
        EV << "Digest not found - returning false" << endl;
    return false;
}


void ReplicaState::addToPrepreparesLog(PBFTPreprepareMessage* msg){

    if (seenMessage(msg)) return;
    preprepares.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTPreprepareMessage" << endl;

}

void ReplicaState::addToPreparesLog(PBFTPrepareMessage* msg){

    if (seenMessage(msg)) return;
    prepares.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTPrepareMessage" << endl;

}

void ReplicaState::addToCommitsLog(PBFTCommitMessage* msg){

    if (seenMessage(msg)) return;
    commits.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTCommitMessage to plain vector" << endl;

}

void ReplicaState::addToRepliesLog(PBFTReplyMessage* msg){

    if (seenMessage(msg)) return;
    replies.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTReplyMessage to plain vector" << endl;
}

bool ReplicaState::seenMessage(cMessage* msg){

    if (dynamic_cast<PBFTRequestMessage*>(msg)){
        PBFTRequestMessage *myMsg = dynamic_cast<PBFTRequestMessage*>(msg);

        for(size_t i=0; i<requests.size(); i++){
            // EV << "input hash: " << myMsg->getOp().getHash() << " requests hash: " << requests.at(i).getOp().getHash() << endl;
            // Check the hash
            // strcmp(requests.at(i).getOp().getHash(), myMsg->getOp().getHash()) == 0
            if (requests.at(i).getOp().getHash() == myMsg->getOp().getHash()){
                if (requests.at(i).getRetryNumber() == myMsg->getRetryNumber()){
                    if(DEBUG)
                        EV << "Request found - returning true" << endl;
                    return true;
                }
            }
        }
    } else if (dynamic_cast<PBFTPreprepareMessage*>(msg)){
        PBFTPreprepareMessage *myMsg = dynamic_cast<PBFTPreprepareMessage*>(msg);

        for(size_t i=0; i<preprepares.size(); i++){

            if (strcmp(preprepares.at(i).getDigest(), myMsg->getDigest()) == 0){
                EV << "Preprepare found - returning true" << endl;
                return true;
            }
        }
    } else if (dynamic_cast<PBFTPrepareMessage*>(msg)){
        PBFTPrepareMessage *myMsg = dynamic_cast<PBFTPrepareMessage*>(msg);

        /**
         * This is different from the others. I will have many PREPARE messages for the same request.
         * I need to be able to count these messages, for each request.
         * Messages are equal if:
         *      - same digest
         *      - same sender
         */
        for(size_t i=0; i<prepares.size(); i++){
            if (strcmp(prepares.at(i).getDigest(), myMsg->getDigest()) == 0 && prepares.at(i).getCreatorKey() == myMsg->getCreatorKey()){
                EV << "Prepare found - returning true" << endl;
                return true;
            }
        }
    } else if (dynamic_cast<PBFTCommitMessage*>(msg)){
        PBFTCommitMessage *myMsg = dynamic_cast<PBFTCommitMessage*>(msg);

        for(size_t i=0; i<commits.size(); i++){
            if (strcmp(commits.at(i).getDigest(), myMsg->getDigest()) == 0 && commits.at(i).getCreatorKey() == myMsg->getCreatorKey()){
                if (commits.at(i).getSeqNumber() == myMsg->getSeqNumber() && commits.at(i).getView() == myMsg->getView()){
                    EV << "Commit found - returning true" << endl;
                    return true;
                }
            }
        }
    } else if (dynamic_cast<PBFTReplyMessage*>(msg)){
        PBFTReplyMessage *myMsg = dynamic_cast<PBFTReplyMessage*>(msg);

        /**
         * A reply has been already seen when it has:
         *      - same replicaNumber
         *      - same operationResult
         *      - same view
         *      - same operation
         *      - same timestamp
         *      - same retryNumber
         */
        for(size_t i=0; i<replies.size(); i++){
            if(replies.at(i).getOp().getHash() == myMsg->getOp().getHash()){
                if(replies.at(i).getView() == myMsg->getView()){
                    if(replies.at(i).getOperationResult() == myMsg->getOperationResult()){
                        if(replies.at(i).getReplicaNumber() == myMsg->getReplicaNumber()){
                            if(replies.at(i).getOp().getTimestamp() == myMsg->getOp().getTimestamp()){
                                if(replies.at(i).getRetryNumber() == myMsg->getRetryNumber()){
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(DEBUG)
        EV << "Message not found - returning false" << endl;

    return false;
}


bool ReplicaState::searchPreparedCertificate(PBFTPrepareMessage* m){
    int prepare_c = 0;
    int preprepare_c = 0;

    for(size_t i=0; i<prepares.size(); i++){
        if(strcmp(prepares.at(i).getDigest(), m->getDigest()) == 0){
            if(prepares.at(i).getSeqNumber() == m->getSeqNumber() && prepares.at(i).getView() == m->getView()){
                if(DEBUG)
                    EV << "prepare_c incremented" << endl;
                prepare_c ++;
            }
        }
    }

    for(size_t i=0; i<preprepares.size(); i++){
        if(strcmp(preprepares.at(i).getDigest(), m->getDigest()) == 0){
            if(preprepares.at(i).getSeqNumber() == m->getSeqNumber() && preprepares.at(i).getView() == m->getView()){
                if(DEBUG)
                    EV << "preprepares_c incremented" << endl;
                preprepare_c ++;
            }
        }
    }

    if (prepare_c == 2*f && preprepare_c == 1){ // TODO exact match on prepare_c, maybe ge than 2f?
        // If exact match, then only once this call will return true, helping to have less COMMIT messages in the network!?
        EV << "Found Prepared Certificate! " << endl;
        return true;
    }

    return false;
}


bool ReplicaState::searchCommittedCertificate(PBFTCommitMessage* m){
    int commits_c = 0;
    bool minePresent = false;

    for(size_t i=0; i<commits.size(); i++){
        if(strcmp(commits.at(i).getDigest(), m->getDigest()) == 0){
            if(commits.at(i).getSeqNumber() == m->getSeqNumber() && commits.at(i).getView() == m->getView()){
                if(DEBUG)
                    EV << "commits_c incremented" << endl;
                commits_c ++;

                if(DEBUG)
                    EV << "Creator key: " << commits.at(i).getCreatorKey() << " overlaykey: " << *overlayk << endl;

                if(commits.at(i).getCreatorKey() == *overlayk){
                    minePresent = true;
                    if(DEBUG)
                        EV << "minePresent is true" << endl;
                }
            }
        }
    }

    if (commits_c == 2*f +1 && minePresent){ // TODO exact match on commits_c, maybe ge than 2f?
        EV << "Found Committed Certificate! block hash: " << m->getDigest() << endl;
        return true;
    }

    return false;
}


bool ReplicaState::otherPreprepareAccepted(PBFTPreprepareMessage* msg){

    for(size_t i=0; i<preprepares.size(); i++){
        if (prepares.at(i).getSeqNumber() == msg->getSeqNumber() && prepares.at(i).getView() == msg->getView()){
            if(strcmp(preprepares.at(i).getDigest(), msg->getDigest()) != 0){
                return true;
            }
        }
    }

    return false;
}

bool ReplicaState::searchReplyCertificate(PBFTReplyMessage* msg){
    int replies_c = 0;

    for(size_t i=0; i<replies.size(); i++){

        if(replies.at(i).getOp().getHash() == msg->getOp().getHash()){
            if(DEBUG)
                EV << "replies_c incremented" << endl;
            replies_c ++;
        }
    }

    if (replies_c == f +1){ // TODO exact match on replies_c, maybe ge than f?
        EV << "Found Reply Certificate! " << endl;
        return true;
    }

    return false;
}

bool ReplicaState::requestHasReply(PBFTRequestMessage* msg){
    /**
     * The reply must have the same:
     *      - operation hash
     *      - operation client
     *
     */
    for(size_t i=0; i<replies.size(); i++){
        if(msg->getOp().getHash() == replies.at(i).getOp().getHash()){
            if(msg->getOp().getOriginatorKey() == replies.at(i).getOp().getOriginatorKey()){
                if(DEBUG)
                    EV << "Request already has a reply at this replica. " << endl;
                return true;
            }
        }
    }

    if(DEBUG)
        EV << "There is not a reply for this request. " << endl;

    return false;
}



