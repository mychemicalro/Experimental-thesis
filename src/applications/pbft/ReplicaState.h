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
    void addToRequestsLog(PBFTRequestMessage* msg);

    /**
     * Check if a given message digest is in requests
     */
    bool digestInRequestsLog(const char* digest);

    /**
     * Add message to preprepares log
     */
    void addToPrepreparesLog(PBFTPreprepareMessage* msg);

    /**
     * Add message to prepares log
     */
    void addToPreparesLog(PBFTPrepareMessage* msg);

    void addToCommitsLog(PBFTCommitMessage* msg);

    void addToRepliesLog(PBFTReplyMessage* msg);

    /**
     * Returns true if the request is already present in requests
     */
    bool seenRequest(cMessage* msg);

    /**
     * Check if there is some PreparedCertificate for some message m.
     * The messages should be unique in the preprepares vector.
     * So a simple count should be enough,
     * where digest, seqNum and v are the same as in the request.
     */
    bool searchPreparedCertificate(PBFTPrepareMessage* m);

    bool searchCommittedCertificate(PBFTCommitMessage* m);

    bool searchReplyCertificate(PBFTReplyMessage* m);

    /**
     * Check if the replica hash accepted another prepare with same seqNum and view but with different digest
     */
    bool otherPreprepareAccepted(PBFTPreprepareMessage* msg);



protected:

private:
    // Class variables
    int currentView;
    bool primary; /* this replica is primary*/

    // TODO Message log
    std::vector<PBFTRequestMessage> requests;
    std::vector<PBFTPreprepareMessage> preprepares;
    std::vector<PBFTPrepareMessage> prepares;
    std::vector<PBFTCommitMessage> commits;
    std::vector<PBFTReplyMessage> replies;

    std::map<const char*, std::vector<PBFTPrepareMessage> > prepares_map;

};


#endif /* REPLICASTATE_H_ */
