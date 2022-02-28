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
 *      - a preprepares vector
 *      - a prepares vector
 *      - a commits vector
 *      - a replies vector
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
    virtual void initializeState(const OverlayKey* ok);

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
     * Returns true if the message is already present in the log
     */
    bool seenMessage(cMessage* msg);

    /**
     * Check if there is some PreparedCertificate for some message m.
     * The messages should be unique in the preprepares vector.
     * So a simple count should be enough,
     * where digest, seqNum and v are the same as in the request.
     */
    bool searchPreparedCertificate(PBFTPrepareMessage* m);

    /**
     * I need to have also my commit message.
     */
    bool searchCommittedCertificate(PBFTCommitMessage* m);

    bool searchReplyCertificate(PBFTReplyMessage* m);

    /**
     * Check if the replica hash accepted another prepare with same seqNum and view but with different digest
     */
    bool otherPreprepareAccepted(PBFTPreprepareMessage* msg);

    /**
     * Returns true if for the request in input there was also a reply -> meaning that the operation is already in the blockchain
     */
    bool requestHasReply(PBFTRequestMessage* msg);

    /**
     * Procedure that check if a replica can send a PREPARE message.
     * Checks all the existing preprepares it received, and if the block can be prepared
     * and the block is not among the candidates, the block becomes a candidate
     */
    bool checkIfCanPrepare(PBFTRequestMessage* msg);

    void addCandidateBlock(PBFTPreprepareMessage* preprep);


    bool isPresentCandidateBlock(PBFTCommitMessage* comm);


    map<string,Block> getCandidateBlocks();

    Block& getBlock(const char* digest);

    PBFTPreprepareMessage& getPreprepareMessage(Operation& op);


protected:

private:
    // Class variables
    int currentView;
    bool primary; /* this replica is primary*/
    int f;

    // TODO Message log
    std::vector<PBFTRequestMessage> requests;
    std::vector<PBFTPreprepareMessage> preprepares;
    std::vector<PBFTPrepareMessage> prepares;
    std::vector<PBFTCommitMessage> commits;
    std::vector<PBFTReplyMessage> replies;

    const OverlayKey* overlayk;

    map<string,Block> candidateBlocks;

};


#endif /* REPLICASTATE_H_ */
