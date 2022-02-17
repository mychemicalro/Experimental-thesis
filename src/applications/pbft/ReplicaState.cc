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

    WATCH(primary);
    WATCH(currentView);

    // TODO
    // WATCH_VECTOR(requests);
    // WATCH_VECTOR(preprepares);

}

void ReplicaState::addToRequestsLog(cMessage* msg){

    if (dynamic_cast<PBFTRequestMessage*>(msg)){
        if (seenRequest(msg)) return;
        // EV << "request size: " << requests.size() << endl;
        PBFTRequestMessage *myMsg = dynamic_cast<PBFTRequestMessage*>(msg);
        requests.push_back(*myMsg);
        EV << "Added a PBFTRequestMessage" << endl;
        // EV << "request size: " << requests.size() << endl;
    }
}

bool ReplicaState::digestInRequestsLog(const char* digest){

    for(int i=0; (int) i<requests.size(); i++){
        if (strcmp(requests.at(i).getOp().computeHash(), digest) == 0){
            EV << "Digest found - returning true" << endl;
            return true;
        }
    }

    EV << "Digest not found - returning true" << endl;
    return false;
}


void ReplicaState::addToPrepreparesLog(cMessage* msg){

    if (dynamic_cast<PBFTPreprepareMessage*>(msg)){
        if (seenRequest(msg)) return;
        // EV << "preprepares size: " << preprepares.size() << endl;
        PBFTPreprepareMessage *myMsg = dynamic_cast<PBFTPreprepareMessage*>(msg);
        preprepares.push_back(*myMsg);
        EV << "Added a PBFTPreprepareMessage" << endl;
        // EV << "preprepares size: " << preprepares.size() << endl;
    }
}


bool ReplicaState::seenRequest(cMessage* msg){

    if (dynamic_cast<PBFTRequestMessage*>(msg)){
        PBFTRequestMessage *myMsg = dynamic_cast<PBFTRequestMessage*>(msg);

        for(int i=0; (int) i<requests.size(); i++){
            EV << "input hash: " << myMsg->getOp().computeHash() << " requests hash: " << requests.at(i).getOp().computeHash() << endl;
            // Check the hash
            if (strcmp(requests.at(i).getOp().computeHash(), myMsg->getOp().computeHash()) == 0){
                EV << "Request found - returning true" << endl;
                return true;
            }
        }
        EV << "Request not found - returning false" << endl;

    } else if (dynamic_cast<PBFTPreprepareMessage*>(msg)){
        PBFTPreprepareMessage *myMsg = dynamic_cast<PBFTPreprepareMessage*>(msg);

        for(int i=0; (int) i<preprepares.size(); i++){
            // TODO make some check
            EV << "Trying to check if I already received this preprepare message" << endl;
            if (strcmp(preprepares.at(i).getDigest(), myMsg->getDigest()) == 0){
                EV << "Preprepare found - returning true" << endl;
                return true;
            }
        }
        EV << "Preprepare not found - returning false" << endl;
    }


    return false;
}




