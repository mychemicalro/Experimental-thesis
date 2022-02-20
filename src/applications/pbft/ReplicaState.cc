/*
 * ReplicaState.cc
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */


#define DEBUG false
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

void ReplicaState::initializeState() {
    EV << "ReplicaState initialized" << endl;
    currentView = 0;
    primary = false;
    requests.clear();
    preprepares.clear();
    prepares.clear();

    WATCH(primary);
    WATCH(currentView);

    // TODO
    // WATCH_VECTOR(requests);
    // WATCH_VECTOR(preprepares);
    // WATCH_VECTOR(prepares);

}

void ReplicaState::addToRequestsLog(PBFTRequestMessage* msg){
    if (seenRequest(msg)) return;
    requests.push_back(*msg);
    EV << "Added a PBFTRequestMessage" << endl;

}

bool ReplicaState::digestInRequestsLog(const char* digest){

    for(size_t i=0; i<requests.size(); i++){
        EV << "request hash: " << requests.at(i).getOp().getHash().c_str() << " digest hash: " << digest << endl;
        if (strcmp(requests.at(i).getOp().getHash().c_str(), digest) == 0){
            EV << "Digest found - returning true" << endl;
            return true;
        }
    }

    EV << "Digest not found - returning false" << endl;
    return false;
}




void ReplicaState::addToPrepreparesLog(PBFTPreprepareMessage* msg){
    if (seenRequest(msg)) return;
    preprepares.push_back(*msg);
    EV << "Added a PBFTPreprepareMessage" << endl;

}

void ReplicaState::addToPreparesLog(PBFTPrepareMessage* msg){

    if (seenRequest(msg)) return;
    prepares.push_back(*msg);
    EV << "Added a PBFTPrepareMessage to plain vector" << endl;
    EV << "prepares size:" << prepares.size() << endl;

    /*
    std::map<const char*, std::vector<PBFTPrepareMessage> >::iterator it = prepares_map.find(msg->getDigest());

    if (it != prepares_map.end()){
        EV << "Found PREPARE message with digest: " << msg->getDigest() << endl;

        EV << "Inserting" << endl;
        it->second.push_back(*msg);
        EV << "Size: "<< it->second.size() << endl;

    } else {
        EV << "Seeing for the first time PREPARE message with digest: " << msg->getDigest() << endl;
        prepares_map.insert(pair<const char*, vector<PBFTPrepareMessage> >(msg->getDigest(), vector<PBFTPrepareMessage>()));
    }
    */
}

void ReplicaState::addToCommitsLog(PBFTCommitMessage* msg){
    if (seenRequest(msg)) return;
    commits.push_back(*msg);
    EV << "Added a PBFTCommitMessage to plain vector" << endl;
    EV << "commits size:" << commits.size() << endl;

}

bool ReplicaState::seenRequest(cMessage* msg){

    if (dynamic_cast<PBFTRequestMessage*>(msg)){
        PBFTRequestMessage *myMsg = dynamic_cast<PBFTRequestMessage*>(msg);

        for(size_t i=0; i<requests.size(); i++){
            EV << "input hash: " << myMsg->getOp().getHash() << " requests hash: " << requests.at(i).getOp().getHash() << endl;
            // Check the hash
            // strcmp(requests.at(i).getOp().getHash(), myMsg->getOp().getHash()) == 0
            if (requests.at(i).getOp().getHash() == myMsg->getOp().getHash()){
                EV << "Request found - returning true" << endl;
                return true;
            }
        }

        EV << "Request not found - returning false" << endl;

    } else if (dynamic_cast<PBFTPreprepareMessage*>(msg)){
        PBFTPreprepareMessage *myMsg = dynamic_cast<PBFTPreprepareMessage*>(msg);

        for(size_t i=0; i<preprepares.size(); i++){
            // TODO make some check
            EV << "Trying to check if I already received this preprepare message" << endl;
            if (strcmp(preprepares.at(i).getDigest(), myMsg->getDigest()) == 0){
                EV << "Preprepare found - returning true" << endl;
                return true;
            }
        }
        EV << "Preprepare not found - returning false" << endl;

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
            // TODO make some check
            EV << "Trying to check if I already received this prepare message" << endl;
            EV << prepares.at(i).getCreatorKey() << " " << myMsg->getCreatorKey() << endl;
            if (strcmp(prepares.at(i).getDigest(), myMsg->getDigest()) == 0 && prepares.at(i).getCreatorKey() == myMsg->getCreatorKey()){
                EV << "Prepare found - returning true" << endl;
                return true;
            }
        }
        EV << "Prepare not found - returning false" << endl;

    } else if (dynamic_cast<PBFTCommitMessage*>(msg)){
        PBFTCommitMessage *myMsg = dynamic_cast<PBFTCommitMessage*>(msg);

        /**
         * Same as prepares
         *
         */
        for(size_t i=0; i<commits.size(); i++){
            // TODO make some check
            EV << "Trying to check if I already received this Commit message" << endl;
            EV << commits.at(i).getCreatorKey() << " " << myMsg->getCreatorKey() << endl;
            if (strcmp(commits.at(i).getDigest(), myMsg->getDigest()) == 0 && commits.at(i).getCreatorKey() == myMsg->getCreatorKey()){
                EV << "Commit found - returning true" << endl;
                return true;
            }

        }
        EV << "Commit not found - returning false" << endl;
    }


    return false;
}


bool ReplicaState::searchPreparedCertificate(PBFTPrepareMessage* m){
    int prepare_c = 0;
    int preprepare_c = 0;
    int f = 1;

    for(size_t i=0; i<prepares.size(); i++){
        if(strcmp(prepares.at(i).getDigest(), m->getDigest()) == 0){
            if(prepares.at(i).getSeqNumber() == m->getSeqNumber() && prepares.at(i).getView() == m->getView()){
                EV << "prepare_c incremented" << endl;
                prepare_c ++;
            }
        }
    }

    for(size_t i=0; i<preprepares.size(); i++){
        if(strcmp(preprepares.at(i).getDigest(), m->getDigest()) == 0){
            if(preprepares.at(i).getSeqNumber() == m->getSeqNumber() && preprepares.at(i).getView() == m->getView()){
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
    int f = 1;

    for(size_t i=0; i<commits.size(); i++){
        if(strcmp(commits.at(i).getDigest(), m->getDigest()) == 0){
            if(commits.at(i).getSeqNumber() == m->getSeqNumber() && commits.at(i).getView() == m->getView()){
                EV << "commits_c incremented" << endl;
                commits_c ++;
            }
        }
    }

    if (commits_c == 2*f +1){ // TODO exact match on commits_c, maybe ge than 2f?
        EV << "Found Committed Certificate! " << endl;
        return true;
    }

    return false;
}

