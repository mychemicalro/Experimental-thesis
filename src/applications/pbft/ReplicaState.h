/*
 * ReplicaState.h
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */

#ifndef REPLICASTATE_H_
#define REPLICASTATE_H_

#include <NodeVector.h>
#include <omnetpp.h>
#include <InitStages.h>
#include <vector>
#include "PBFTMessage_m.h"

/**
 * Class for storing the replica state and the message log, that will have to contain
 * most of the client requests and lots of other messages.
 * Initially, the replica is not primary.
 *
 * It maintains:
 *      - a requests vector
 *      -
 *
 */

class ReplicaState : public cSimpleModule {

public:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage* msg);

    // don't know what it is
    virtual int numInitStages() const {
        return MAX_STAGE_OVERLAY + 1;
    }

    // initialize parameters and data structure
    virtual void initializeState();

    // Getters
    int getCurrentView(){ return currentView; }
    bool getPrimary() { return primary; }

    // and setters
    void setCurrentView(int cv){ currentView = cv; }
    void setPrimary(bool b){ primary = b; }

    /**
     * Add message to requests log
     */
    void addToRequestsLog(cMessage* msg);

    /**
     * Check if a given message digest is in requests
     */
    bool digestInRequestsLog(const char* digest);

    /**
     * Add message to preprepares log
     */
    void addToPrepreparesLog(cMessage* msg);

    /**
     * Returns true if the request is already present in requests
     */
    bool seenRequest(cMessage* msg);

protected:

private:
    // Class variables
    int currentView;
    bool primary; /* this replica is primary*/

    // TODO Message log
    std::vector<PBFTRequestMessage> requests;
    std::vector<PBFTPreprepareMessage> preprepares;
};


#endif /* REPLICASTATE_H_ */
